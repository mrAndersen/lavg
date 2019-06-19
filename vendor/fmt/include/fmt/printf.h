                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 (arg_index == std::numeric_limits<unsigned>::max())
    return this->do_get_arg(this->parse_context().next_arg_id());
  return base::get_arg(arg_index - 1);
}

template <typename OutputIt, typename Char, typename AF>
unsigned basic_printf_context<OutputIt, Char, AF>::parse_header(
  iterator &it, format_specs &spec) {
  unsigned arg_index = std::numeric_limits<unsigned>::max();
  char_type c = *it;
  if (c >= '0' && c <= '9') {
    // Parse an argument index (if followed by '$') or a width possibly
    // preceded with '0' flag(s).
    internal::error_handler eh;
    unsigned value = parse_nonnegative_int(it, eh);
    if (*it == '$') {  // value is an argument index
      ++it;
      arg_index = value;
    } else {
      if (c == '0')
        spec.fill_ = '0';
      if (value != 0) {
        // Nonzero value means that we parsed width and don't need to
        // parse it or flags again, so return now.
        spec.width_ = value;
        return arg_index;
      }
    }
  }
  parse_flags(spec, it);
  // Parse width.
  if (*it >= '0' && *it <= '9') {
    internal::error_handler eh;
    spec.width_ = parse_nonnegative_int(it, eh);
  } else if (*it == '*') {
    ++it;
    spec.width_ = visit_format_arg(
          internal::printf_width_handler<char_type>(spec), get_arg(it));
  }
  return arg_index;
}

template <typename OutputIt, typename Char, typename AF>
void basic_printf_context<OutputIt, Char, AF>::format() {
  auto &buffer = internal::get_container(this->out());
  auto start = iterator(this->parse_context());
  auto it = start;
  using internal::pointer_from;
  while (*it) {
    char_type c = *it++;
    if (c != '%') continue;
    if (*it == c) {
      buffer.append(pointer_from(start), pointer_from(it));
      start = ++it;
      continue;
    }
    buffer.append(pointer_from(start), pointer_from(it) - 1);

    format_specs spec;
    spec.align_ = ALIGN_RIGHT;

    // Parse argument index, flags and width.
    unsigned arg_index = parse_header(it, spec);

    // Parse precision.
    if (*it == '.') {
      ++it;
      if ('0' <= *it && *it <= '9') {
        internal::error_handler eh;
        spec.precision = static_cast<int>(parse_nonnegative_int(it, eh));
      } else if (*it == '*') {
        ++it;
        spec.precision =
            visit_format_arg(internal::printf_precision_handler(), get_arg(it));
      } else {
        spec.precision = 0;
      }
    }

    format_arg arg = get_arg(it, arg_index);
    if (spec.has(HASH_FLAG) && visit_format_arg(internal::is_zero_int(), arg))
      spec.flags = static_cast<uint_least8_t>(spec.flags & (~internal::to_unsigned<int>(HASH_FLAG)));
    if (spec.fill_ == '0') {
      if (arg.is_arithmetic())
        spec.align_ = ALIGN_NUMERIC;
      else
        spec.fill_ = ' ';  // Ignore '0' flag for non-numeric types.
    }

    // Parse length and convert the argument to the required type.
    using internal::convert_arg;
    switch (*it++) {
    case 'h':
      if (*it == 'h')
        convert_arg<signed char>(arg, *++it);
      else
        convert_arg<short>(arg, *it);
      break;
    case 'l':
      if (*it == 'l')
        convert_arg<long long>(arg, *++it);
      else
        convert_arg<long>(arg, *it);
      break;
    case 'j':
      convert_arg<intmax_t>(arg, *it);
      break;
    case 'z':
      convert_arg<std::size_t>(arg, *it);
      break;
    case 't':
      convert_arg<std::ptrdiff_t>(arg, *it);
      break;
    case 'L':
      // printf produces garbage when 'L' is omitted for long double, no
      // need to do the same.
      break;
    default:
      --it;
      convert_arg<void>(arg, *it);
    }

    // Parse type.
    if (!*it)
      FMT_THROW(format_error("invalid format string"));
    spec.type = static_cast<char>(*it++);
    if (arg.is_integral()) {
      // Normalize type.
      switch (spec.type) {
      case 'i': case 'u':
        spec.type = 'd';
        break;
      case 'c':
        // TODO: handle wchar_t better?
        visit_format_arg(
              internal::char_converter<basic_printf_context>(arg), arg);
        break;
      }
    }

    start = it;

    // Format argument.
    visit_format_arg(AF(buffer, spec, *this), arg);
  }
  buffer.append(pointer_from(start), pointer_from(it));
}

template <typename Buffer>
struct basic_printf_context_t {
  typedef basic_printf_context<
    std::back_insert_iterator<Buffer>, typename Buffer::value_type> type;
};

typedef basic_printf_context_t<internal::buffer>::type printf_context;
typedef basic_printf_context_t<internal::wbuffer>::type wprintf_context;

typedef basic_format_args<printf_context> printf_args;
typedef basic_format_args<wprintf_context> wprintf_args;

/**
  \rst
  Constructs an `~fmt::format_arg_store` object that contains references to
  arguments and can be implicitly converted to `~fmt::printf_args`. 
  \endrst
 */
template<typename... Args>
inline format_arg_store<printf_context, Args...>
  make_printf_args(const Args &... args) { return {args...}; }

/**
  \rst
  Constructs an `~fmt::format_arg_store` object that contains references to
  arguments and can be implicitly converted to `~fmt::wprintf_args`. 
  \endrst
 */
template<typename... Args>
inline format_arg_store<wprintf_context, Args...>
  make_wprintf_args(const Args &... args) { return {args...}; }

template <typename S, typename Char = FMT_CHAR(S)>
inline std::basic_string<Char>
vsprintf(const S &format,
         basic_format_args<typename basic_printf_context_t<
           internal::basic_buffer<Char>>::type> args) {
  basic_memory_buffer<Char> buffer;
  printf(buffer, to_string_view(format), args);
  return to_string(buffer);
}

/**
  \rst
  Formats arguments and returns the result as a string.

  **Example**::

    std::string message = fmt::sprintf("The answer is %d", 42);
  \endrst
*/
template <typename S, typename... Args>
inline FMT_ENABLE_IF_T(
    internal::is_string<S>::value, std::basic_string<FMT_CHAR(S)>)
    sprintf(const S &format, const Args & ... args) {
  internal::check_format_string<Args...>(format);
  typedef internal::basic_buffer<FMT_CHAR(S)> buffer;
  typedef typename basic_printf_context_t<buffer>::type context;
  format_arg_store<context, Args...> as{ args... };
  return vsprintf(to_string_view(format),
                  basic_format_args<context>(as));
}

template <typename S, typename Char = FMT_CHAR(S)>
inline int vfprintf(std::FILE *f, const S &format,
                    basic_format_args<typename basic_printf_context_t<
                      internal::basic_buffer<Char>>::type> args) {
  basic_memory_buffer<Char> buffer;
  printf(buffer, to_string_view(format), args);
  std::size_t size = buffer.size();
  return std::fwrite(
    buffer.data(), sizeof(Char), size, f) < size ? -1 : static_cast<int>(size);
}

/**
  \rst
  Prints formatted data to the file *f*.

  **Example**::

    fmt::fprintf(stderr, "Don't %s!", "panic");
  \endrst
 */
template <typename S, typename... Args>
inline FMT_ENABLE_IF_T(internal::is_string<S>::value, int)
    fprintf(std::FILE *f, const S &format, const Args & ... args) {
  internal::check_format_string<Args...>(format);
  typedef internal::basic_buffer<FMT_CHAR(S)> buffer;
  typedef typename basic_printf_context_t<buffer>::type context;
  format_arg_store<context, Args...> as{ args... };
  return vfprintf(f, to_string_view(format),
                  basic_format_args<context>(as));
}

template <typename S, typename Char = FMT_CHAR(S)>
inline int vprintf(const S &format,
                   basic_format_args<typename basic_printf_context_t<
                    internal::basic_buffer<Char>>::type> args) {
  return vfprintf(stdout, to_string_view(format), args);
}

/**
  \rst
  Prints formatted data to ``stdout``.

  **Example**::

    fmt::printf("Elapsed time: %.2f seconds", 1.23);
  \endrst
 */
template <typename S, typename... Args>
inline FMT_ENABLE_IF_T(internal::is_string<S>::value, int)
    printf(const S &format_str, const Args & ... args) {
  internal::check_format_string<Args...>(format_str);
  typedef internal::basic_buffer<FMT_CHAR(S)> buffer;
  typedef typename basic_printf_context_t<buffer>::type context;
  format_arg_store<context, Args...> as{ args... };
  return vprintf(to_string_view(format_str),
                 basic_format_args<context>(as));
}

template <typename S, typename Char = FMT_CHAR(S)>
inline int vfprintf(std::basic_ostream<Char> &os,
                    const S &format,
                    basic_format_args<typename basic_printf_context_t<
                      internal::basic_buffer<Char>>::type> args) {
  basic_memory_buffer<Char> buffer;
  printf(buffer, to_string_view(format), args);
  internal::write(os, buffer);
  return static_cast<int>(buffer.size());
}

/**
  \rst
  Prints formatted data to the stream *os*.

  **Example**::

    fmt::fprintf(cerr, "Don't %s!", "panic");
  \endrst
 */
template <typename S, typename... Args>
inline FMT_ENABLE_IF_T(internal::is_string<S>::value, int)
    fprintf(std::basic_ostream<FMT_CHAR(S)> &os,
            const S &format_str, const Args & ... args) {
  internal::check_format_string<Args...>(format_str);
  typedef internal::basic_buffer<FMT_CHAR(S)> buffer;
  typedef typename basic_printf_context_t<buffer>::type context;
  format_arg_store<context, Args...> as{ args... };
  return vfprintf(os, to_string_view(format_str),
                  basic_format_args<context>(as));
}
FMT_END_NAMESPACE

#endif  // FMT_PRINTF_H_
