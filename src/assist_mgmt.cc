#include "assist_mgmt.h"

//TODO:Error code handling
//TODO:Error reporting

namespace kagami {
  static MemoryDisposer
    disposer = nullptr;
  static ObjectTypeFetcher
    type_fetcher    = nullptr;
  static ErrorInformer 
    error_informer  = nullptr;
  static DescriptorFetcher
    desc_fetcher = nullptr;
  static ArrayElementFetcher
    arr_elem_fetcher = nullptr;
  static ObjectDumper
    dumper = nullptr;
  static CapacityInformer
    cap_informer = nullptr;

  MemoryDisposer GetMemoryDisposer() { return disposer; }
  ObjectDumper GetObjectDumper() { return dumper; }
  DescriptorFetcher GetDescriptorFetcher() { return desc_fetcher; }

  vector<string> BuildStringVector(string source) {
    vector<string> result;
    string temp;
    for (auto unit : source) {
      if (unit == '|') {
        result.push_back(temp);
        temp.clear();
        continue;
      }
      temp.append(1, unit);
    }
    if (temp != "") result.push_back(temp);
    return result;
  }

  string CombineStringVector(vector<string> target) {
    string result;
    for (size_t i = 0; i < target.size(); ++i) {
      result = result + target[i] + "|";
    }
    result.pop_back();
    return result;
  }

  bool InformCallbackFacilities(ExtInterfaces &interfaces) {
    desc_fetcher = interfaces.desc_fetcher;
    arr_elem_fetcher = interfaces.arr_elem_fetcher;
    dumper = interfaces.dumper;
    type_fetcher = interfaces.type_fetcher;
    cap_informer = interfaces.capacity_informer;
    disposer = interfaces.disposer;
    error_informer = interfaces.error_informer;
    desc_fetcher = interfaces.desc_fetcher;

    return (desc_fetcher != nullptr)
      && (arr_elem_fetcher != nullptr)
      && (dumper != nullptr)
      && (type_fetcher != nullptr)
      && (cap_informer != nullptr)
      && (disposer != nullptr)
      && (error_informer != nullptr)
      && (desc_fetcher != nullptr);
  }

  DescriptorValue GetDesciptor(string id, void *obj_map) {
    Descriptor descriptor;
    int result = desc_fetcher(&descriptor, obj_map, id.data());
    if (result != 1) return nullopt;
    return descriptor;
  }

  size_t GetArrayObjectCapacity(Descriptor desc) {
    return cap_informer(desc);
  }

  DescriptorValue GetArrayElementDescriptor(Descriptor arr_desc, size_t index) {
    Descriptor elem;
    int fetch_result = arr_elem_fetcher(&arr_desc, &elem, index);
    if (fetch_result != 1) return nullopt;
    return elem;
  }

  vector<Descriptor> GetArrayElements(Descriptor desc) {
    auto size = cap_informer(desc);
    vector<Descriptor> result;
    for (size_t index = 0; index < size; index += 1) {
      result.push_back(GetArrayElementDescriptor(desc, index).value());
    }
    return result;
  }

  void ReturnIntObject(int64_t value, VMState state) {
    state.tunnel(&value, state.ret_slot, kExtTypeInt);
  }

  void ReturnFloatObject(double value, VMState state) {
    state.tunnel(&value, state.ret_slot, kExtTypeFloat);
  }

  void ReturnBoolObject(bool value, VMState state) {
    int buffer = value ? 1 : 0;
    state.tunnel(&buffer, state.ret_slot, kExtTypeBool);
  }

  void ReturnStringObject(string value, VMState state) {
    state.tunnel(value.data(), state.ret_slot, kExtTypeString);
  }

  void ReturnWideStringObject(wstring value, VMState state) {
    state.tunnel(value.data(), state.ret_slot, kExtTypeString);
  }

  void ReturnFunctionPointerObject(GenericFunctionPointer value, VMState state) {
    state.tunnel(&value, state.ret_slot, kExtTypeFunctionPointer);
  }

  void ReturnObjPointerObject(GenericPointer value, VMState state) {
    state.tunnel(&value, state.ret_slot, kExtTypeObjectPointer);
  }

  void ThrowError(string msg, VMState state) {
    error_informer(state.vm, msg.data());
  }

  ObjectType GetObjectType(void *obj_map, string id) {
    return ObjectType(type_fetcher(obj_map, id.data()));
  }
}