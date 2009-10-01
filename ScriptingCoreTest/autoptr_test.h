#include "AutoPtr.h"

class TestObject
{
	public:
		int refCount;

		TestObject() : refCount(0) {}
		void addRef() { refCount++; }
		int release() { refCount--; return refCount; }
		bool AccessMe() { return true; }
};

TEST (AutoPtrTest)
{
  FB::AutoPtr<TestObject> ptr;
	FB::AutoPtr<TestObject> ptr2;
	ptr = new TestObject;

	CHECK (ptr->refCount == 1);
	ptr2 = ptr;
	CHECK (ptr->refCount == 2);
	ptr2 = NULL;
	CHECK (ptr->refCount == 1);

	CHECK ((*ptr).AccessMe() == true);
	CHECK (ptr->AccessMe() == true);
}