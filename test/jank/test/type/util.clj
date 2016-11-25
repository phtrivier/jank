(ns jank.test.type.all
  (:require [clojure.test :refer :all]
            [jank.test.bootstrap :refer :all :refer-macros :all]))

(def error #"type error:")

(defn test-file [file]
  (let [full-file (str file ".jank")]
    (println "[type] testing" full-file)
    (if (should-fail? full-file)
      (is (thrown-with-msg? AssertionError
                            error
                            (valid-type? full-file)))
      (is (valid-type? full-file)))))


(deftest overloaded-lambdas
  (doseq [file ["fail-multiple-definition"
                "fail-return-type"
                "pass-different-param-count"
                "pass-same-param-count"]]
    (test-file (str "test/type/lambda/overload/" file))))

(deftest lambda-returns
  (doseq [file ["fail-lambda-wrong-type"
                "fail-no-return"
                "fail-unknown-type"
                "fail-unknown-value"
                "fail-wrong-param-type"
                "fail-wrong-type"
                "fail-first-class-lambda-wrong-return-type"
                "fail-first-class-lambda-wrong-parameter-type"
                "pass-lambda"
                "pass-normal"
                "pass-param"
                "pass-void-no-return"
                "pass-void-incomplete-if"
                "pass-void-wrong-type"]]
    (test-file (str "test/type/lambda/return/" file))))

(deftest lambda-type-deduction
  (doseq [file ["fail-mismatched-types"
                "pass-if"
                "pass-void"
                "pass-with-normal-return"
                ; TODO: lambda identifiers
                ;"pass-with-unicode"
                "pass-void-lambda"
                "pass-non-void-lambda"
                "pass-deduced-lambda"]]
    (test-file (str "test/type/lambda/deduce/" file))))

(deftest closures
  (doseq [file ["pass-global-from-nested"
                "pass-global"
                "pass-local-from-nested"
                "pass-parameter-from-nested"
                "pass-partial"]]
    (test-file (str "test/type/lambda/closure/" file))))

(deftest superpositions
  ; TODO
  (doseq [file [;"pass-lazy"
                ;"pass-outer-and-inner"
                ;"pass-outer"
                ;"pass-parameter"
                ]]
    (test-file (str "test/type/lambda/superposition/" file))))

(deftest type-declarations
  ; TODO
  (doseq [file [;"fail-invalid-generic"
                ;"pass-generic"
                ;"pass-generic-multiple"
                "pass-multiple"
                "pass-normal"]]
    (test-file (str "test/type/declaration/type/" file))))

(deftest binding-declarations
  ; TODO
  (doseq [file ["fail-mismatched-types"
                "fail-unknown-type"
                "pass-lambda"
                "pass-multiple"
                "pass-normal"]]
    (test-file (str "test/type/declaration/binding/" file))))

(deftest structs
  ; TODO: Fix these
  (doseq [file [;"fail-name-used"
                "fail-multiple-definition"
                "fail-invalid-member-type"
                "fail-members-same-name"
                ;"fail-member-function-redefinition"
                ;"fail-member-function-declaration-incorrect"
                ;"fail-extern-declaration"
                "pass-single-member"
                "pass-multiple-members"
                "pass-extern-type-member"
                "pass-struct-member"
                "pass-recursive"
                "pass-declaration"
                "pass-member-declaration"]]
    (test-file (str "test/type/struct/" file))))

(deftest new-expressions
  (doseq [file ["fail-incorrect-value-types"
                "fail-no-values"
                "fail-not-enough-values"
                "fail-too-many-types"
                "fail-too-many-values"
                "fail-unknown-type"
                "pass-as-parameter"
                "pass-constructor"
                "pass-correct-values"
                "pass-value-expressions"
                "pass-binding-explicit"
                "pass-binding-implicit"]]
    (test-file (str "test/type/new/" file))))
