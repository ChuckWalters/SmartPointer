#pragma once
#include <windows.h>
#include <map>
#include <queue>

///<summary>
/// Contains a critical section wrapper class and thread safe STL collection wrappers.
///</summary>
namespace SafeCollections
{
	///<summary>
	/// Encapsulates critical sections
	///</summary>
	class CriticalSection
	{
	public:
		CriticalSection()
		{
			InitLock();
		}
		void Lock(void)
		{
			EnterCriticalSection(&mMutex);
		}
		void UnLock(void)
		{
			LeaveCriticalSection(&mMutex);
		}
	protected:
		void InitLock(void)
		{
			InitializeCriticalSection(&mMutex);
		}
		void DeleteLock(void)
		{
			DeleteCriticalSection(&mMutex);
		}
	private:
		CRITICAL_SECTION  
			mMutex;
	};
	///<summary>
	/// A thread safe STL map
	///</summary>
	///<remarks>
	/// Sample Declaration:
	///     typedef SmartNode<cMyClass> cMyClassNode;
	///
	///		cMap<int,cMyClassNode> myMap;
	///
	/// Sample Use:
	///     cMyClass * myClass = new cMyClass(); // any class of yours
	///		cMyClassNode node(myClass);
	///		myMap.Add(&node);
	///</remarks>
	template <class T_Key, class T_NodeData>
	class SafeMap : public CriticalSection
	{
	public:
		SafeMap(void):CriticalSection()
		{
		}
		virtual ~SafeMap(void)
		{
			Lock();
			tMap::iterator it;
			if (!mMap.empty())
			{
				for (it = mMap.begin(); it != mMap.end();)
				{
					Remove(it->first);
					it = mMap.begin();
				}
			}
			UnLock();
			DeleteLock();
		}
		virtual bool Add(T_Key &key, T_NodeData * data)
		{
			if (!data)
				return(FALSE);
			Lock();
			mMap.insert(tMap::value_type(key,*data));
			UnLock();
			return(TRUE);
		}

		virtual bool Remove(const T_Key &key, T_NodeData &data)
		{
			Lock();
			if (Find(key,data))
			{
				mMap.erase(key);
				UnLock();
				return (TRUE);
			}
			UnLock();
			return(FALSE);
		}
	  
		virtual bool Remove(const T_Key &key)
		{
			Lock();
			if (Find(key))
			{
				mMap.erase(key);
				UnLock();
				return (TRUE);
			}
			UnLock();
			return(FALSE);
		}
		virtual bool Find(const T_Key &key, T_NodeData &data)
		{
			Lock();
			if (!mMap.empty())
			{
				tMap::iterator it = mMap.find(key);
				if (it != mMap.end())
				{
					data = (*it).second;
					UnLock();
					return (TRUE);
				}
			}
			UnLock();
			return(FALSE);
		}
		virtual bool Find(const T_Key &key)
		{
			Lock();
			if (!mMap.empty())
			{
				tMap::iterator it = mMap.find(key);
				if (it != mMap.end())
				{
					UnLock();
					return (TRUE);
				}
			}
			UnLock();
			return(FALSE);
		}
		virtual int Count(void)const
		{
			return(mMap.size());
		}
	protected:
		typedef std::map<T_Key, T_NodeData> tMap;
		tMap mMap;
	};
	///<summary>
	/// A thread safe wrapper for std::queue<> 
	///</summary>
	///<remarks>
	/// Supports reference counting smart pointer nodes
	///</remarks>
	template <class T_NodeData>
	class SafeQ : public CriticalSection
	{
	public:
		SafeQ(void):CriticalSection()
		{
		}
		virtual ~SafeQ(void)
		{
			Lock();
			if (!mQ.empty())
			{
				Pop();
			}
			UnLock();
			DeleteLock();
		}
		virtual void Push(T_NodeData &data)
		{
			Lock();
			mQ.push(data);
			UnLock();
		}

		virtual T_NodeData Pop()
		{
			Lock();
			T_NodeData data = mQ.front();
			mQ.pop();
			UnLock();
			return(data);
		}
		virtual int Count(void)const
		{
			return((int)mQ.size());
		}

	protected:
		typedef std::queue<T_NodeData> tQ;
		tQ	mQ;
	};
}
