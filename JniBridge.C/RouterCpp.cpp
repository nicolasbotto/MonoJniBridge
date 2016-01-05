#include "org_mule_api_jni_Bridge.h"
#include "JniManager.h"

using namespace std;

static JniManager* jniManager;
static jobject javaRouter;
static MonoDomain* monoDomain;
static MonoObject* routerInstance;
static MonoMethod* processMethod;

string executingFolder;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
	jniManager = new JniManager(vm);
	JNIEnv* env = jniManager->getEnv();

	if (!env)
	{
		return JNI_ERR;
	}

	// point to the relevant directories of the Mono installation
	mono_set_dirs("C:/Program Files (x86)/Mono/lib/", "C:/Program Files (x86)/Mono/etc/");

	// load the default Mono configuration file in 'etc/mono/config'
	mono_config_parse(nullptr);

	//	MonoDomain* monoDomain = mono_jit_init_version("embedding_mono_domain", "v4.0.30319");

	// initialize the root domain which will hold corlib and will always be alive
	try
	{
		monoDomain = mono_jit_init_version("jni Root Domain", "v4.0.30319");

		// open our Example.dll assembly
		MonoAssembly* assembly = mono_domain_assembly_open(monoDomain, "D:/MyStuff/Embedded mono/JniBridge/Debug/Org.Mule.Api.Routing.dll");
		MonoImage* monoImage = mono_assembly_get_image(assembly);

		// find the Entity class in the image
		MonoClass* routerClass = mono_class_from_name(monoImage, "Org.Mule.Api.Routing", "Router");

		// allocate memory for one Entity instance
		routerInstance = mono_object_new(monoDomain, routerClass);
		mono_runtime_object_init(routerInstance);

		// find the Process method that takes zero parameters
		processMethod = mono_class_get_method_from_name(routerClass,"Process", 1);

		jniManager->setMono(monoDomain, monoImage);
	}
	catch (char* S)
	{
		printf(S);
	}

	return JNI_VERSION_1_6;
}

// The VM calls JNI_OnUnload when the class loader containing the native library is garbage collected.
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
	//remove global references
	jniManager->cleanup();
}

JNIEXPORT void JNICALL Java_org_mule_api_jni_Bridge_init
(JNIEnv *env, jobject obj)
{
	jniManager->setRouter(obj);
}

JNIEXPORT jobject JNICALL Java_org_mule_api_jni_Bridge_invokeNetMethod
(JNIEnv *env, jobject obj, jobject request)
{
	MonoObject* exception = nullptr;

	// create a MonoString that will be passed to the constructor as an argument
	//MonoString* name = mono_string_new(mono_domain_get(), "Giorgos");

	MonoObject* processRequest = jniManager->toProcessRequest(request);

	void* args[1];
	args[0] = processRequest;

	mono_runtime_invoke(processMethod, routerInstance, args, &exception);

	// check for any thrown exception
	if (exception)
	{
		const char* message = mono_string_to_utf8(mono_object_to_string(exception, nullptr));
		jniManager->throwException(message);
		return nullptr;
	}

	jobject result = jniManager->getObject(request);

	TypeConverter* converter = new TypeConverter();
	converter->init(env);

	std::vector<byte> cString = converter->convertToC<std::vector<byte>>(env, result);

	const char* x = cString.size() + "";
	
	return env->NewStringUTF(x);
}
