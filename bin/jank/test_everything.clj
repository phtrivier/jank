#!/usr/bin/env bb

(ns jank.test-everything
  (:require
   [jank.compiler+runtime.core]
   [jank.util :as util]))

(defn show-env []
  (util/log-info "JANK_BUILD_TYPE: " (System/getenv "JANK_BUILD_TYPE"))
  (util/log-info "JANK_LINT: " (System/getenv "JANK_LINT"))
  (util/log-info "JANK_COVERAGE: " (System/getenv "JANK_COVERAGE"))
  (util/log-info "JANK_ANALYZE: " (System/getenv "JANK_ANALYZE"))
  (util/log-info "JANK_SANITIZE: " (System/getenv "JANK_SANITIZE")))

(def os->deps-cmd {"Linux" "sudo apt-get install -y curl git git-lfs zip build-essential entr libssl-dev libdouble-conversion-dev pkg-config ninja-build cmake zlib1g-dev libffi-dev libzip-dev libbz2-dev doctest-dev libboost-all-dev gcc g++ libgc-dev"

                   "Mac OS X" "brew install curl git git-lfs zip entr openssl double-conversion pkg-config ninja python cmake gnupg zlib doctest boost libzip lbzip2 llvm@19"})

(defmulti install-deps
  (fn []
    (System/getProperty "os.name")))

(defmethod install-deps "Linux" []
  (let [apt? (try
               (util/quiet-shell {} "which apt-get")
               true
               (catch Exception _e
                 false))]
    (if-not apt?
      (util/log-warning "Skipping dependency install, since we don't have apt-get")
      (do
        ; Install deps required for running our tests.
        (util/quiet-shell {} "sudo apt-get install -y default-jdk software-properties-common lsb-release npm lcov leiningen")
        (util/quiet-shell {} "sudo npm install --global @chrisoakman/standard-clojure-style")

        ; Install jank's build deps.
        (util/quiet-shell {} (os->deps-cmd "Linux"))

        ; Install Clang/LLVM.
        (util/quiet-shell {} "curl -L -O https://apt.llvm.org/llvm.sh")
        (util/quiet-shell {} "chmod +x llvm.sh")
        (util/quiet-shell {} (str "sudo ./llvm.sh " util/llvm-version " all"))
        ; The libc++abi headers conflict with the system headers:
        ; https://github.com/llvm/llvm-project/issues/121300
        (util/quiet-shell {} (str "sudo apt-get remove -y libc++abi-" util/llvm-version "-dev"))

        ; Install the new Clojure CLI.
        (util/quiet-shell {} "curl -L -O https://github.com/clojure/brew-install/releases/latest/download/linux-install.sh")
        (util/quiet-shell {} "chmod +x linux-install.sh")
        (util/quiet-shell {} "sudo ./linux-install.sh")))))

(defmethod install-deps "Mac OS X" []
  (util/quiet-shell {:extra-env {"HOMEBREW_NO_AUTO_UPDATE" "1"}}
                    (os->deps-cmd "Mac OS X")))

(defn -main [{:keys [install-deps? validate-formatting? compiler+runtime]}]
  (util/log-boundary "Environment")
  (show-env)

  (util/log-boundary "Install dependencies")
  (if-not install-deps?
    (util/log-info "Not enabled")
    (util/with-elapsed-time duration
      (install-deps)
      (util/log-info-with-time duration "Dependencies installed")))

  (jank.compiler+runtime.core/-main {:validate-formatting? validate-formatting?
                                     :build? (:build? compiler+runtime)}))

(when (= *file* (System/getProperty "babashka.file"))
  (-main {:install-deps? (parse-boolean (or (System/getenv "JANK_INSTALL_DEPS") "true"))
          :validate-formatting? (parse-boolean (or (System/getenv "JANK_LINT") "true"))
          :compiler+runtime {:build? (some? (System/getenv "JANK_BUILD_TYPE"))}}))
