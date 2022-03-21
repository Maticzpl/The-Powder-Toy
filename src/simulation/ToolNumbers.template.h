#ifdef TOOL_NUMBERS_CALL
# define TOOL_DEFINE(name) tools.push_back(SimTool()), tools.back().Tool_ ## name ();
#endif
#ifdef TOOL_NUMBERS_DECLARE
# define TOOL_DEFINE(name) void Tool_ ## name ();
#endif

@tool_defs@

#undef TOOL_DEFINE
