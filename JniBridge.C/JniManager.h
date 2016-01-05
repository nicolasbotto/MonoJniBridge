#include <jni.h>
#include <map>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/environment.h>
#include <mono/jit/jit.h>
#include "TypeConverter.h"

using namespace std;

class JniManager
{
private:
	MonoDomain* monoDomain;
	TypeConverter* typeConverter;
	JavaVM* jvm;
	jmethodID getAssemblyName;
	jmethodID getAssemblyPath;
	jmethodID getMethodName;
	jmethodID getLog;
	jmethodID getNotifyEvents;
	jmethodID getFullTrust;
	jmethodID getIsSingleton;
	jmethodID getMethodArguments;
	jmethodID getInvocationProperties;
	jmethodID getSessionProperties;
	jmethodID getOutboundProperties;
	jmethodID getInboundProperties;
	jmethodID responseCtor;
	jmethodID setPayloadMethod;
	jmethodID logMethod;
	jmethodID instrumentMethod;
	jmethodID setInvocationProperties;
	jmethodID setSessionProperties;
	jmethodID setOutboundProperties;
	jclass responseClazz;
	jobject routerInstance;
	jclass exceptionClazz;

	jmethodID getResult;

	MonoMethod* processMethod;

	// Fields
	MonoMethod* setAssemblyPathField;
	MonoMethod* setMethodNameField;
	MonoMethod* setAssemblyNameField;
	MonoMethod* setFullTrustField;
	MonoMethod* setIsSingletonField;
	MonoMethod* setLogField;
	MonoMethod* setNotifyEventsField;
	MonoMethod* setMethodArgumentsField;
	MonoMethod* setInboundPropertiesField;
	MonoMethod* setInvocationPropertiesField;
	MonoMethod* setOutboundPropertiesField;
	MonoMethod* setSessionPropertiesField;

	MonoClass* processRequestClass;
	

public:

	JniManager(JavaVM*);
	void setMono(MonoDomain*, MonoImage*);
	void init();
	JNIEnv* getEnv();
	jobject toResponseObject(jobject);
	void cleanup();
	void setRouter(jobject);
	void checkJniException();
	MonoObject* toProcessRequest(jobject);
	jobject getObject(jobject);
	void throwException(const char*);
};
