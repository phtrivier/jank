#include <iostream>
#include <sstream>

#include <jank/runtime/util.hpp>
#include <jank/runtime/hash.hpp>
#include <jank/runtime/obj/function.hpp>
#include <jank/runtime/obj/map.hpp>
#include <jank/runtime/obj/vector.hpp>
#include <jank/runtime/behavior/seqable.hpp>

namespace jank::runtime::obj
{
  map::map(runtime::detail::persistent_map &&d)
    : data{ std::move(d) }
  { }
  map::map(runtime::detail::persistent_map const &d)
    : data{ d }
  { }

  native_box<map> map::create(runtime::detail::persistent_map const &o)
  { return jank::make_box<map>(o); }

  template <typename It>
  struct map_iterator_wrapper : behavior::sequence
  {
    map_iterator_wrapper() = default;
    map_iterator_wrapper(object_ptr c, It const &b, It const &e)
      : coll{ c }, begin{ b }, end{ e }
    { }

    void to_string_impl(fmt::memory_buffer &buff) const
    {
      auto inserter(std::back_inserter(buff));
      format_to(inserter, "(");
      for(auto i(begin); i != end; ++i)
      {
        format_to(inserter, "[");
        (*i).first->to_string(buff);
        format_to(inserter, " ");
        (*i).second->to_string(buff);
        format_to(inserter, "]");
        auto n(i);
        if(++n != end)
        { format_to(inserter, " "); }
      }
      format_to(inserter, ")");
    }
    void to_string(fmt::memory_buffer &buff) const override
    { return to_string_impl(buff); }
    native_string to_string() const override
    {
      fmt::memory_buffer buff;
      to_string_impl(buff);
      return native_string{ buff.data(), buff.size() };
    }
    native_integer to_hash() const override
    /* TODO: Hash from contents. */
    { return reinterpret_cast<native_integer>(this); }

    sequence_ptr seq() const override
    { return static_cast<sequence_ptr>(const_cast<map_iterator_wrapper<It>*>(this)); }

    object_ptr first() const override
    { return jank::make_box<vector>(runtime::detail::peristent_vector{ begin->first, begin->second }); }
    behavior::sequence_ptr next() const override
    {
      auto n(begin);
      ++n;

      if(n == end)
      { return nullptr; }

      return jank::make_box<map_iterator_wrapper<It>>(coll, n, end);
    }
    behavior::sequence* next_in_place() override
    {
      ++begin;

      if(begin == end)
      { return nullptr; }

      return this;
    }
    object_ptr next_in_place_first() override
    {
      ++begin;

      if(begin == end)
      { return nullptr; }

      return jank::make_box<vector>(runtime::detail::peristent_vector{ begin->first, begin->second });
    }

    object_ptr coll{};
    It begin, end;
  };

  native_bool map::equal(object const &o) const
  {
    auto const *m(o.as_map());
    if(!m)
    { return false; }

    return to_hash() == m->to_hash();
  }

  void to_string_impl
  (
    runtime::detail::persistent_map::const_iterator const &begin,
    runtime::detail::persistent_map::const_iterator const &end,
    fmt::memory_buffer &buff
  )
  {
    auto inserter(std::back_inserter(buff));
    inserter = '{';
    for(auto i(begin); i != end; ++i)
    {
      i->first->to_string(buff);
      inserter = ' ';
      i->second->to_string(buff);
      auto n(i);
      if(++n != end)
      {
        inserter = ',';
        inserter = ' ';
      }
    }
    inserter = '}';
  }
  void map::to_string(fmt::memory_buffer &buff) const
  { to_string_impl(data.begin(), data.end(), buff); }
  native_string map::to_string() const
  {
    fmt::memory_buffer buff;
    to_string_impl(data.begin(), data.end(), buff);
    return native_string{ buff.data(), buff.size() };
  }
  /* TODO: Cache this. */
  native_integer map::to_hash() const
  {
    auto seed(static_cast<native_integer>(data.size()));
    for(auto const &e : data)
    {
      seed = runtime::detail::hash_combine(seed, *e.first);
      seed = runtime::detail::hash_combine(seed, *e.second);
    }
    return seed;
  }
  map const* map::as_map() const
  { return this; }
  behavior::seqable const* map::as_seqable() const
  { return this; }
  behavior::sequence_ptr map::seq() const
  {
    if(data.size() == 0)
    { return nullptr; }
    return jank::make_box<map_iterator_wrapper<runtime::detail::persistent_map::const_iterator>>(const_cast<map*>(this), data.begin(), data.end());
  }

  size_t map::count() const
  { return data.size(); }

  object_ptr map::with_meta(object_ptr m) const
  {
    validate_meta(m);
    auto ret(jank::make_box<map>(data));
    ret->meta = m;
    return ret;
  }

  behavior::metadatable const* map::as_metadatable() const
  { return this; }
}
