#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <substrate.h>
#include <string>

struct Item
{
	void** vtable;
	static Item* mItems[512];
	static Item* mEnderpearl;
	static void initItems();
	static void initCreativeItems();
	static void addCreativeItem(Item*, short);
	
	Item* setIcon(const std::string&, int);
};
struct EnderpearlItem : public Item
{
	char data[86]; // 90 bytes, just to be safe
	EnderpearlItem(const std::string&, int);
};

void (*_initItems)();
void initItems()
{
	Item::mItems[368] = Item::mEnderpearl = (new EnderpearlItem("ender_pearl", 368 - 0x100))->setIcon("ender_pearl", 0);
	
	_initItems();
}

void (*_initCreativeItems)();
void initCreativeItems()
{
	_initCreativeItems();
	
	/* Set category via vtable because it's the least code */
	void (*setCategory)(Item*, int) = (void (*)(Item*, int)) Item::mEnderpearl->vtable[5];
	setCategory(Item::mEnderpearl, 3);
	
	Item::addCreativeItem(Item::mItems[368], 0);
}

int getEnderManDrop()
{
	return 368;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	MSHookFunction((void*) &Item::initItems, (void*) &initItems, (void**) &_initItems);
	MSHookFunction((void*) &Item::initCreativeItems, (void*) &initCreativeItems, (void**) &_initCreativeItems);
	
	/* Make EnderMan drop pearls */
	void** EnderMan_vtable = (void**) ((uintptr_t) dlsym(RTLD_DEFAULT, "_ZTV8EnderMan") + 8);
	EnderMan_vtable[193] = (void*) &getEnderManDrop;

	return JNI_VERSION_1_2;
}
