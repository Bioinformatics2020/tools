#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogJsonParser, Log, All);
//下面实现单例类 declare
#define JSONPARSERANDDISPATHER_DECLARE_SINGLE_INSTANCE(CLASS_TYPE)  \
public: \
static CLASS_TYPE& GetInstance(); \
static void Destory(); \
private: \
CLASS_TYPE() = default; \
~CLASS_TYPE() = default; \
CLASS_TYPE(const CLASS_TYPE&) = delete; \
CLASS_TYPE& operator=(const CLASS_TYPE&) = delete; \
static CLASS_TYPE* m_sInstance;

#define JSONPARSERANDDISPATHER_IMPLEMENT_SINGLE_INSTANCE(CLASS_TYPE)  \
CLASS_TYPE* CLASS_TYPE::m_sInstance = nullptr; \
CLASS_TYPE& CLASS_TYPE::GetInstance() \
{ \
if (m_sInstance == nullptr) \
{ \
m_sInstance = new CLASS_TYPE(); \
} \
return *m_sInstance; \
} \
void CLASS_TYPE::Destory() \
{ \
if (m_sInstance != nullptr) \
{ \
delete m_sInstance; \
m_sInstance = nullptr; \
} \
}

#define REGREST_MESSAGE(ADDR, ID, STRUCT, FUNC) \
UJsonMessageSubsystem::GetInstance().Register(ADDR, \
			FMessageResponse(ID,STRUCT::StaticStruct(), \
				FMessageResponseDelegate::CreateRaw( this,&FUNC)));

#define REGREST_MESSAGE_UObject(ADDR, ID, STRUCT, FUNC) \
UJsonMessageSubsystem::GetInstance().Register(ADDR, \
FMessageResponse(ID,STRUCT::StaticStruct(), \
FMessageResponseDelegate::CreateUObject( this,&FUNC)));

#define UNREGREST_MESSAGE(ADDR, ID) \
UJsonMessageSubsystem::UnRegister(ADDR,ID);