#ifdef LANGUAGES_DECLARE
# define LANGUAGE_DEFINE(code) extern const unsigned char language_ ## code[]; extern const unsigned int language_ ## code ## Size;
#endif
#ifdef LANGUAGES_ENUMERATE
# define LANGUAGE_DEFINE(code) { #code, language_ ## code, language_ ## code ## Size },
#endif

@language_defs@

#undef LANGUAGE_DEFINE
