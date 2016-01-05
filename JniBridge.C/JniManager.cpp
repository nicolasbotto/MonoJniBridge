#include "JniManager.h"

JniManager::JniManager(JavaVM* vm)
{
	assert(vm);
	jvm = vm;
	init();
}

void JniManager::setMono(MonoDomain* domain, MonoImage* monoImage)
{
	monoDomain = domain;

	processRequestClass = mono_class_from_name(monoImage, "Org.Mule.Api.Routing", "ProcessRequest");
	setAssemblyNameField = mono_class_get_method_from_name(processRequestClass, "set_AssemblyName", 1);
	setMethodNameField = mono_class_get_method_from_name(processRequestClass, "set_MethodName", 1);
	setAssemblyPathField = mono_class_get_method_from_name(processRequestClass, "set_AssemblyPath", 1);
	setFullTrustField = mono_class_get_method_from_name(processRequestClass, "set_FullTrust", 1);
	setIsSingletonField = mono_class_get_method_from_name(processRequestClass, "set_IsSingleton", 1);
	setLogField = mono_class_get_method_from_name(processRequestClass, "set_Log", 1);
	setMethodArgumentsField = mono_class_get_method_from_name(processRequestClass, "set_MethodArguments", 1);
	setNotifyEventsField = mono_class_get_method_from_name(processRequestClass, "set_NotifyEvents", 1);
	setInboundPropertiesField = mono_class_get_method_from_name(processRequestClass, "set_InboundProperties", 1);
	setInvocationPropertiesField = mono_class_get_method_from_name(processRequestClass, "set_InvocationProperties", 1);
	setSessionPropertiesField = mono_class_get_method_from_name(processRequestClass, "set_SessionProperties", 1);
	setOutboundPropertiesField = mono_class_get_method_from_name(processRequestClass, "set_OutboundProperties", 1);
}

void JniManager::init()
{
	JNIEnv* env = getEnv();

	assert(env);

	typeConverter = new TypeConverter();
	typeConverter->init(env);

	// Exception class
	jclass exceptionClazzTemp = env->FindClass("java/lang/Exception");

	if (exceptionClazzTemp == NULL)
	{
		checkJniException();
	}

	exceptionClazz = (jclass)env->NewGlobalRef(exceptionClazzTemp);

	env->DeleteLocalRef(exceptionClazzTemp);
	/*
	// org/mule/api/jniRouter
	jclass routerClazzTmp = env->FindClass("org/mule/api/jni/Bridge");

	checkJniException();

	logMethod = env->GetMethodID(routerClazzTmp, "log", "(Ljava/lang/String;)V");

	checkJniException();

	instrumentMethod = env->GetMethodID(routerClazzTmp, "instrument", "(Ljava/lang/String;I)V");

	checkJniException();

	env->DeleteLocalRef(routerClazzTmp);
	*/
	// org/mule/api/jni/Response
	jclass responseClazzTmp = env->FindClass("org/mule/api/jni/Response");

	checkJniException();

	responseClazz = (jclass)env->NewGlobalRef(responseClazzTmp);
	env->DeleteLocalRef(responseClazzTmp);

	responseCtor = env->GetMethodID(responseClazz, "<init>", "()V");

	checkJniException();

	setPayloadMethod = env->GetMethodID(responseClazz, "setPayload", "(Ljava/lang/Object;)V");

	checkJniException();

	setInvocationProperties = env->GetMethodID(responseClazz, "setInvocationProperties", "(Ljava/util/Map;)V");

	checkJniException();

	setSessionProperties = env->GetMethodID(responseClazz, "setSessionProperties", "(Ljava/util/Map;)V");

	checkJniException();

	setOutboundProperties = env->GetMethodID(responseClazz, "setOutboundProperties", "(Ljava/util/Map;)V");

	checkJniException();

	// org/mule/api/jni/Request

	jclass processRequestClazz = env->FindClass("org/mule/api/jni/Request");

	checkJniException();

	getAssemblyName = env->GetMethodID(processRequestClazz, "getAssemblyName", "()Ljava/lang/String;");

	checkJniException();

	getAssemblyPath = env->GetMethodID(processRequestClazz, "getAssemblyPath", "()Ljava/lang/String;");

	checkJniException();

	getMethodName = env->GetMethodID(processRequestClazz, "getMethodName", "()Ljava/lang/String;");

	checkJniException();

	getLog = env->GetMethodID(processRequestClazz, "getLog", "()Z");

	checkJniException();

	getNotifyEvents = env->GetMethodID(processRequestClazz, "getNotifyEvents", "()Z");

	checkJniException();

	getFullTrust = env->GetMethodID(processRequestClazz, "getFullTrust", "()Z");

	checkJniException();

	getIsSingleton = env->GetMethodID(processRequestClazz, "getIsSingleton", "()Z");

	checkJniException();

	getInboundProperties = env->GetMethodID(processRequestClazz, "getInboundProperties", "()Ljava/util/Map;");

	checkJniException();

	getMethodArguments = env->GetMethodID(processRequestClazz, "getMethodArguments", "()Ljava/util/Map;");

	checkJniException();

	getInvocationProperties = env->GetMethodID(processRequestClazz, "getInvocationProperties", "()Ljava/util/Map;");

	checkJniException();

	getSessionProperties = env->GetMethodID(processRequestClazz, "getSessionProperties", "()Ljava/util/Map;");

	checkJniException();

	getOutboundProperties = env->GetMethodID(processRequestClazz, "getOutboundProperties", "()Ljava/util/Map;");

	checkJniException();

	getResult = env->GetMethodID(processRequestClazz, "getResult", "()Ljava/lang/Object;");

	checkJniException();
}

void JniManager::setRouter(jobject obj)
{
	JNIEnv* env = getEnv();

	assert(env);

	routerInstance = env->NewGlobalRef(obj);
}

JNIEnv* JniManager::getEnv()
{
	JNIEnv *env;
	assert(jvm);
	int envStatus = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);

	if (envStatus == JNI_OK)
	{
		jvm->AttachCurrentThread((void**)&env, NULL);
		return env;
	}

	return NULL;
}

void JniManager::cleanup()
{
	JNIEnv* env = getEnv();

	assert(env);


	env->DeleteGlobalRef(responseClazz);
	env->DeleteGlobalRef(routerInstance);

	typeConverter->cleanup(env);

	checkJniException();
}

jobject JniManager::toResponseObject(jobject request)
{
	JNIEnv* env = getEnv();

	assert(env);

	jobject response = env->NewObject(responseClazz, responseCtor);
	
	jobject jPayload;

	//if (request->Result != nullptr)
	//{
	//	// If it's a string or byte[]
	//	if (payload->GetType()->ToString() == "System.String")
	//	{
	//		msclr::interop::marshal_context ctx;
	//		const char* convertedPayload = ctx.marshal_as<const char*>((String^)payload);
	//		jPayload = env->NewStringUTF(convertedPayload);
	//	}
	//	else
	//	{
	//		jPayload = typeConverter->convertToJavaArray<jbyteArray>(env, payload);
	//	}

	//	env->CallVoidMethod(response, setPayloadMethod, jPayload);
	//	env->DeleteLocalRef(jPayload);
	//}

	// set Mule Message properties
	/*jobject jInvocationProperties = typeConverter->convertToJavaMap(env, request->InvocationProperties);
	jobject jSessionProperties = typeConverter->convertToJavaMap(env, request->SessionProperties);
	jobject jOutboundProperties = typeConverter->convertToJavaMap(env, request->OutboundProperties);
	
	env->CallVoidMethod(response, setInvocationProperties, jInvocationProperties);
	env->CallVoidMethod(response, setOutboundProperties, jOutboundProperties);
	env->CallVoidMethod(response, setSessionProperties, jSessionProperties);

	env->DeleteLocalRef(jInvocationProperties);
	env->DeleteLocalRef(jOutboundProperties);
	env->DeleteLocalRef(jSessionProperties);*/
	
	return response;
}

void JniManager::checkJniException()
{
	JNIEnv* env = getEnv();
	assert(env);

	jthrowable error = env->ExceptionOccurred();

	if (error)
	{
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

void JniManager::throwException(const char* message)
{
	JNIEnv* env = getEnv();
	assert(env);
	env->ThrowNew(exceptionClazz, message);
}

jobject JniManager::getObject(jobject obj)
{
	JNIEnv* env = getEnv();

	assert(env);

	return env->CallObjectMethod(obj, getResult);
}

MonoObject* JniManager::toProcessRequest(jobject obj)
{
	JNIEnv* env = getEnv();
	MonoObject* processRequest = mono_object_new(monoDomain, processRequestClass);
	mono_runtime_object_init(processRequest);

	MonoString* assemblyName = mono_string_new(monoDomain, typeConverter->convertToC<const char*>(env, env->CallObjectMethod(obj, getAssemblyName, "()Ljava/lang/String;")));
	MonoString* assemblyPath = mono_string_new(monoDomain, typeConverter->convertToC<const char*>(env, env->CallObjectMethod(obj, getAssemblyPath, "()Ljava/lang/String;")));
	MonoString* methodName = mono_string_new(monoDomain, typeConverter->convertToC<const char*>(env, env->CallObjectMethod(obj, getMethodName, "()Ljava/lang/String;")));
	bool fullTrust = env->CallBooleanMethod(obj, getFullTrust);
	bool isSingleton = env->CallBooleanMethod(obj, getIsSingleton);
	bool log = env->CallBooleanMethod(obj, getLog);
	bool notifyEvents = env->CallBooleanMethod(obj, getNotifyEvents);

	MonoObject* exception = NULL;

	//MonoString* name = mono_string_new(monoDomain, assemblyName);
	
	void* args[] = { assemblyName };
	mono_runtime_invoke(setAssemblyNameField, processRequest, args, &exception);
	
	args[0] = { assemblyPath };
	mono_runtime_invoke(setAssemblyPathField, processRequest, args, &exception);
	
	args[0] = { methodName };
	mono_runtime_invoke(setMethodNameField, processRequest, args, &exception);
	
	args[0] = { &fullTrust };
	mono_runtime_invoke(setFullTrustField, processRequest, args, &exception);
	
	args[0] = { &log };
	mono_runtime_invoke(setLogField, processRequest, args, &exception);
	
	args[0] = { &isSingleton };
	mono_runtime_invoke(setIsSingletonField, processRequest, args, &exception);
	
	args[0] = { &notifyEvents };
	mono_runtime_invoke(setNotifyEventsField, processRequest, args, &exception);
	
	if (exception)
	{
		const char* message = mono_string_to_utf8(mono_object_to_string(exception, nullptr));
		throwException(message);
	}

	//request->MethodArguments = typeConverter->convertToC<Dictionary<String^, Object^>^>(env, env->CallObjectMethod(obj, getMethodArguments));
	//request->InboundProperties = typeConverter->convertToC<Dictionary<String^, Object^>^>(env, env->CallObjectMethod(obj, getInboundProperties));
	//request->InvocationProperties = typeConverter->convertToC<Dictionary<String^, Object^>^>(env, env->CallObjectMethod(obj, getInvocationProperties));
	//request->OutboundProperties = typeConverter->convertToC<Dictionary<String^, Object^>^>(env, env->CallObjectMethod(obj, getOutboundProperties));
	//request->SessionProperties = typeConverter->convertToC<Dictionary<String^, Object^>^>(env, env->CallObjectMethod(obj, getSessionProperties));
	
	return processRequest;
}
