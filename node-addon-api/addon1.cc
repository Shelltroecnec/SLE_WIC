// hello.cc using Node-API
#include <node_api.h>
#include <assert.h>

napi_value Method(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value value;

  size_t argc = 1;
  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, NULL, "Wrong number of arguments");
    return NULL;
  }

  bool is_array = 0;
  status = napi_is_array(env, args[0], &is_array);
  assert(status == napi_ok);

  if (!is_array) {
    napi_throw_type_error(env, NULL, "Argument type should be an array");
    return NULL;
  }

  // args[0] equals to something like
  // [1, 2, 3, 4, 5]
  // which is the user input (js array)
  napi_value array = args[0];

  if (!array) {
    std::cout<<"array is null"<<std::endl;
    return NULL;
  }


  uint32_t length;
  status = napi_get_array_length(env, array, &length);

  for (uint32_t i = 0; i < length; ++i) {
    napi_value ret;
    napi_get_element(env, array, i, &ret);

    int32_t myint;
    napi_get_value_int32(env, ret, &myint);


    napi_value result;
    napi_create_int32(env, myint * 2, &result);
    napi_set_element(env, array, i, result);
  }

  return array;
}

// copy pasted
napi_value init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, nullptr, 0, Method, nullptr, &fn);
  if (status != napi_ok) return nullptr;

  status = napi_set_named_property(env, exports, "hello", fn);
  if (status != napi_ok) return nullptr;
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

