#include "extension_sample.h"

#ifdef _WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

int kagami_LoadExtension(kagami::ExtInterfaces *interfaces) {
  bool facilities_result = kagami::InformCallbackFacilities(
    interfaces->launcher, interfaces->type_fetcher);
  bool mem_mgmt_result = kagami::InformMemoryMgmtInterface(
    interfaces->disposer, interfaces->group_disposer);
  bool error_throwing_result = kagami::InformErrorThrowingInterface(
    interfaces->error_informer);
  int result = facilities_result && mem_mgmt_result && error_throwing_result ?
    1 : 0;
  return result;
}

const char *kagami_ParameterInformer(const char *id) {
  static unordered_map<string, const char *> parameters = {
    FunctionParameters("sample_helloworld", ""),
    FunctionParameters("sample_plus", "a|b"),
    FunctionParameters("sample_throw_error", "")
  };

  auto it = parameters.find(string(id));
  if (it != parameters.end()) return it->second;
  return nullptr;
}

int sample_helloworld(kagami::VMState state) {
  string msg = "Nothing here!";
  kagami::ReturnStringObject(msg, state);
  return 1;
}

int sample_plus(kagami::VMState state) {
  auto a = kagami::FromIntObject("a", state.obj_map);
  auto b = kagami::FromIntObject("b", state.obj_map);
  if (!a.has_value() || !b.has_value()) return 0;
  int64_t result = a.value() + b.value();
  kagami::ReturnIntObject(result, state);
  return 1;
}

int sample_throw_error(kagami::VMState state) {
  kagami::ThrowError("Error Example", state);
  return 1;
}
