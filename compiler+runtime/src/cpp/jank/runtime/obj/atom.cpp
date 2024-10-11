#include <jank/runtime/obj/atom.hpp>

namespace jank::runtime
{
  obj::atom::static_object(object_ptr const o)
    : val{ o }
  {
    assert(val);
  }

  native_bool obj::atom::equal(object const &o) const
  {
    return &o == &base;
  }

  native_persistent_string obj::atom::to_string() const
  {
    fmt::memory_buffer buff;
    to_string(buff);
    return native_persistent_string{ buff.data(), buff.size() };
  }

  void obj::atom::to_string(fmt::memory_buffer &buff) const
  {
    fmt::format_to(std::back_inserter(buff),
                   "{}@{}",
                   magic_enum::enum_name(base.type),
                   fmt::ptr(&base));
  }

  native_persistent_string obj::atom::to_code_string() const
  {
    return to_string();
  }

  native_hash obj::atom::to_hash() const
  {
    return static_cast<native_hash>(reinterpret_cast<uintptr_t>(this));
  }

  object_ptr obj::atom::deref() const
  {
    return val.load();
  }

  object_ptr obj::atom::reset(object_ptr const o)
  {
    assert(o);
    val = o;
    return o;
  }

  obj::persistent_vector_ptr obj::atom::reset_vals(object_ptr const o)
  {
    while(true)
    {
      auto v(val.load());
      if(val.compare_exchange_weak(v, o))
      {
        return make_box<obj::persistent_vector>(std::in_place, v, o);
      }
    }
  }

  /* NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap) */
  object_ptr obj::atom::swap(object_ptr const fn)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(dynamic_call(fn, v));
      if(val.compare_exchange_weak(v, next))
      {
        return next;
      }
    }
  }

  /* NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap) */
  object_ptr obj::atom::swap(object_ptr fn, object_ptr a1)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(dynamic_call(fn, v, a1));
      if(val.compare_exchange_weak(v, next))
      {
        return next;
      }
    }
  }

  /* NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap) */
  object_ptr obj::atom::swap(object_ptr fn, object_ptr a1, object_ptr a2)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(dynamic_call(fn, v, a1, a2));
      if(val.compare_exchange_weak(v, next))
      {
        return next;
      }
    }
  }

  /* NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap) */
  object_ptr obj::atom::swap(object_ptr fn, object_ptr a1, object_ptr a2, object_ptr rest)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(apply_to(fn, conj(a1, conj(a2, rest))));
      if(val.compare_exchange_weak(v, next))
      {
        return next;
      }
    }
  }

  obj::persistent_vector_ptr obj::atom::swap_vals(object_ptr const fn)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(dynamic_call(fn, v));
      if(val.compare_exchange_weak(v, next))
      {
        return make_box<obj::persistent_vector>(std::in_place, v, next);
      }
    }
  }

  obj::persistent_vector_ptr obj::atom::swap_vals(object_ptr fn, object_ptr a1)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(dynamic_call(fn, v, a1));
      if(val.compare_exchange_weak(v, next))
      {
        return make_box<obj::persistent_vector>(std::in_place, v, next);
      }
    }
  }

  obj::persistent_vector_ptr obj::atom::swap_vals(object_ptr fn, object_ptr a1, object_ptr a2)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(dynamic_call(fn, v, a1, a2));
      if(val.compare_exchange_weak(v, next))
      {
        return make_box<obj::persistent_vector>(std::in_place, v, next);
      }
    }
  }

  obj::persistent_vector_ptr
  obj::atom::swap_vals(object_ptr fn, object_ptr a1, object_ptr a2, object_ptr rest)
  {
    while(true)
    {
      auto v(val.load());
      auto const next(apply_to(fn, conj(a1, conj(a2, rest))));
      if(val.compare_exchange_weak(v, next))
      {
        return make_box<obj::persistent_vector>(std::in_place, v, next);
      }
    }
  }

  object_ptr obj::atom::compare_and_set(object_ptr old_val, object_ptr new_val)
  {
    object *old{ old_val };
    return make_box(val.compare_exchange_weak(old, new_val));
  }
}
