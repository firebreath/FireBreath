/**********************************************************\ 
Original Author: Don Jordan (kc7zax)

Created:    Sept 26, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Don Jordan, Firebreath development team
\**********************************************************/

#include "AutoPtr.h"

class TestObject
{
    public:
        int refCount;

        TestObject() : refCount(0) {}
        void addRef() { refCount++; }
        int release()
    {
        refCount--;
        if (refCount == 0)
        {
            delete this;
            return 0;
        }

        return refCount;
    }
        bool AccessMe() { return true; }
};

TEST (AutoPtrTest)
{
    PRINT_TESTNAME;

    FB::AutoPtr<TestObject> ptr;
    FB::AutoPtr<TestObject> ptr2;
    FB::AutoPtr<TestObject> ptr3;
    ptr = new TestObject;
    ptr3 = new TestObject;

    CHECK (ptr->refCount == 1);
    ptr2 = ptr;
    CHECK (ptr->refCount == 2);
    ptr2 = NULL;
    CHECK (ptr->refCount == 1);

    CHECK (ptr3->refCount == 1);
    ptr2 = ptr;
    ptr = ptr3;
    CHECK (ptr3->refCount == 2);
    CHECK (ptr2->refCount == 1);

    ptr = ptr2;
    ptr2 = ptr;
    CHECK (ptr2->refCount == 2);

    CHECK ((*ptr).AccessMe() == true);
    CHECK (ptr->AccessMe() == true);
}