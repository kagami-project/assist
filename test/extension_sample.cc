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
  bool result = kagami::InformCallbackFacilities(*interfaces);                                                                                                               
  return result ? 1 : 0;
}

const char *kagami_ParameterInformer(const char *id) {
  static unordered_map<string, const char *> parameters = {
    FunctionParameters("sample_helloworld", ""),
    FunctionParameters("sample_plus", "a|b"),
    FunctionParameters("sample_throw_error", ""),
    FunctionParameters("sample_variable_print", "@obj")
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
  auto a = kagami::FromObject<kagami::kExtTypeInt>("a", state.obj_map);
  auto b = kagami::FromObject<kagami::kExtTypeInt>("b", state.obj_map);
  if (!kagami::HasValue(a, b)) return 0;
  int64_t result = a.value() + b.value();
  kagami::ReturnIntObject(result, state);
  return 1;
}

int sample_throw_error(kagami::VMState state) {
  kagami::ThrowError("Error Example", state);
  return 0;
}

int sample_variable_print(kagami::VMState state) {
  auto arr = kagami::GetDesciptor("obj", state.obj_map).value();
  auto size = kagami::GetArrayObjectCapacity(arr);
  auto obj = kagami::GetArrayElements(arr);

  for (auto &unit : obj) {
    auto num = kagami::FromDescriptor<int64_t>(unit);
    string msg = "(Extension sample)" + std::to_string(num.value());
    std::puts(msg.data());
  }

  kagami::ReturnIntObject(size, state);
  return 1;
}
