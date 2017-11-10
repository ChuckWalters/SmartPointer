#pragma once
#include <windows.h>

///<summary>
/// 
///</summary>
namespace SmartPointer
{
	///<summary>
	/// This class encapsulates another class derived from cRefCount.
	///</summary>
	///<remarks>
	/// Recomended not using this class directly, rather create cNode<yourClass> objects.
	///</remarks
	template<class T>
	class SmartPtr
	{
	public:
		SmartPtr(T* ptr = 0)
			:mPtr(ptr)
		{
			Init();
		}
		SmartPtr(const SmartPtr& rhs)
			:mPtr(rhs.mPtr)
		{
			Init();
		}
		~SmartPtr()
		{
			if (mPtr)
				mPtr->DecRef();
		}

		SmartPtr& operator=(const SmartPtr& rhs)
		{
			if (mPtr != rhs.mPtr)
			{
				if (mPtr)
					mPtr->DecRef();
				mPtr = rhs.mPtr;
				Init();
			}
			return(*this);
		}

		T* operator->()const
		{
			return(mPtr);
		}
		T& operator*()const
		{
			return(*mPtr);
		}

	private:
		T* mPtr;

		void Init()
		{
			if (mPtr==NULL)
				return;
			mPtr->IncRef();
		}
	};

	///<summary>
	//  Tracks number of references currently held to this object.
	//  Destructs itself when the object loses it's last reference.
	///</summary>
	///<remarks>
	/// This object should only be used by SmartNode
	///</remarks>
	class RefCount
	{
	public:
		void IncRef(void)
		{
			InterlockedIncrement((long*)&mCount);
			//std::cerr<<"RefCount inc, post inc cnt = "<<m_Count<<std::endl;
		}
		void DecRef(void)
		{
			if (InterlockedDecrement ((long*) &mCount) == 0)
			{
				//std::cerr<<"RefCount delete this"<<std::endl;
				delete this;
			}
		}
		long GetRefCount()
		{
			return mCount;
		}
	protected:
		/// Constructors & Destructors not publicly visible

		RefCount(void) : mCount(0)
		{
			//std::cerr<<"RefCount constructor"<<std::endl;
		}

		RefCount(const RefCount& rhs) : mCount(0)
		{
			//std::cerr<<"RefCount copy constructor"<<std::endl;
		}

		RefCount& operator=(const RefCount& rhs)
		{
			//std::cerr<<"RefCount = op"<<std::endl;
			return(*this);
		}
		virtual ~RefCount(void)
		{
			//std::cerr<<"~RefCount"<<std::endl;
		}

	private:
		long volatile mCount;
	};

	///<summary>
	/// Holds an instance of a reference counting smart pointer
	///</summary>
	///<remarks>
	/// Sample Declaration:
	///     typedef SmartNode<MyClass> MyClassNode;
	/// Note that void MyClass::Release() must be defined and that in most cases
	/// it will simply call "delete this;".  An alternative use is to return the
	/// object to an object pool.
	///</remarks>
	template <class T_NodeData>
	class SmartNode
	{
	private:
		struct sNode_Data : public RefCount
		{
			T_NodeData *mData;
			void init(T_NodeData * data)
			{ mData = data;}
	    
			sNode_Data(T_NodeData *data)
			{ init(data);}
	    
			sNode_Data(const sNode_Data& rhs)
			{ init(rhs.mData);}
	    
			~sNode_Data()
			{ mData->Release(); }
		};

		SmartPtr<sNode_Data> mSmartPtr;

	public:
		SmartNode(T_NodeData *data) : mSmartPtr(new sNode_Data(data))
		{}
		SmartNode(void) : mSmartPtr()
		{}

		T_NodeData * GetData(void)
		{ 
			return(mSmartPtr->mData);
		}

		T_NodeData * operator->()const
		{
			return(mSmartPtr->mData);
		}

	};
}
