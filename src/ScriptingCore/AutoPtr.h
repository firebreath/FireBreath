/**********************************************************\ 
Original Author: Don Jordan (kc7zax)

Created:    Sept 26, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Don Jordan, Firebreath development team
\**********************************************************/

#ifndef H_FB_AUTOPTR
#define H_FB_AUTOPTR

namespace FB
{
    template <class T>
    class AutoPtr
    {
        private:
            void Assign(T *obj);

        protected:
            T* object;

        public:
            //Default Constructor
            AutoPtr() : object(NULL) {}
            // Constructor with pointer
            AutoPtr(T *obj) : object(NULL)
            {
                Assign(obj);
            }
            //Copy Constructor
            AutoPtr(const AutoPtr<T>& copy) : object(NULL)
            {
                Assign(copy.object);
            }
            //Destructor
            ~AutoPtr()
            {
                Assign(NULL);
            }

            //Assignment operators
            inline AutoPtr<T>& operator =(T *obj)
            {
                Assign(obj);
                return *this;
            }
            inline AutoPtr<T>& operator =(const AutoPtr<T>& p)
            {
                Assign(p.object);
                return *this;
            }

            //Accessors
            inline T& operator *() const
            {
                return *object;
            }
            inline T* operator ->() const
            {
                return object;
            }

            //Others
            inline bool operator !()
            {
                return !(object);
            }
            inline bool operator ==(const T* obj) const
            {
                return (object == obj);
            }
            inline bool operator ==(const AutoPtr<T>& p) const
            {
                return (object == p.object);
            }
            inline bool operator !=(const T* obj) const
            {
                return (object != obj);
            }
            inline bool operator !=(const AutoPtr<T>& p) const
            {
                return (object != p.object);
            }
            inline T* ptr() const
            {
                return object;
            }
            inline operator T*() const
            {
                return ptr();
            }
    };

    template <class T>
    void AutoPtr<T>::Assign(T *obj)
    {
        if (obj)
            obj->AddRef();
        if (object)
            object->Release();
        object = obj;
    }
}

#endif