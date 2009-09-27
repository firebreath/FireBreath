#pragma once

template <class T>
class AutoPtr
{
	protected:
		T* object;

	public:
		//Default Constructor
		AutoPtr() { object = NULL; }
		//Copy Constructor
		AutoPtr(const AutoPtr<T>& copy)
		{
			object = NULL;
			*this = copy;
		}
		//Destructor
		~AutoPtr()
		{
			if (object)
			{
				object->release();
				if (object->refCount() == 0)
				{
					delete object;
					object = NULL;
				}
			}
		}

		//Assignment operators
		inline AutoPtr<T>& operator =(T *obj)
		{
			//addRef on new one before release on old one.
			if (obj)
				obj->addRef();
			if (object)
				object->release();
			object = obj;

			return *this;
		}
		inline AutoPtr<T>& operator =(const AutoPtr<T>& p)
		{
			//addRef on new one before release on old one.
			if (p.object)
				p.object->addRef();
			if (object)
				object->release();
			object = p.object;

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
};