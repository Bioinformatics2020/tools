#pragma once
enum EMediaStatus:int8
{
	CODESUCCESS = 0,		 //*  0：请求成功。
	CODELOGINFAILED = -1,    //* -1：鉴权错误、登录状态过期、设备鉴权不通过等原因，需尝试重新登陆。
	CODENOLogin = -2,        //* -2：未登录、未鉴权，需要重新登陆
	CODENeterror = -3,       //* -3：服务器或网络异常
	CODEParametererror = -4, //* -4: 请求参数有误
	CODELoginFailed = -5,    //* -5: 登陆失败
	CODEOtherError = -6      //* -6: 其他异常
};

//注意这里枚举的顺序不可改变，牵扯到蓝图的旧代码
UENUM(BlueprintType)
enum class EMediaSource:uint8
{
	EMediaSourceQQMusic = 0,
	EMediaSourceRadio,
	EMediaSourceAudioBooks,
	EMediaSourceBT,
	EMediaSourceUSB,
	EMediaSourceCount
};

UENUM(BlueprintType)
enum class EMediaQQMusicPage:uint8
{
	EMediaQQMusicA1MainPage = 0,
	EMediaQQMusicA2A3RecSongsPage,
	EMediaQQMusicA4SongsDetailPage,
	EMediaQQMusicA5SelectionPage,
	EMediaQQMusicA6SelectionDetailPage,
	EMediaQQMusicA7MoreSingerPage,
	EMediaQQMusicA8A9SongDetailPage,
	EMediaQQMusicA9d2bumDetailPage,
	EMediaQQMusicA10SearchPage,
	EMediaQQMusicA14LoginPage,
	EMediaQQMusicA15NoNetworkPage,
	EMediaQQMusicA16NetworkInstabilityPage,
	EMediaQQMusicA17LoadingPage,
	EMediaQQMusicA18FavoriteSubscriptionsPage,
	EMediaQQMusicA20FavoriteSongsPage,
	EMediaQQMusicA21FavoritePlaylistPage,
	EMediaQQMusicA22CreatePlaylistPage,
	EMediaQQMusicA23SubscriptionSongPage,
	EMediaQQMusicA28PlayerDetailPage,
	EMediaQQMusicListen,
	EMediaQQMusicListenPage,
	EMediaQQMusicStartLogin,
	EMediaQQMusicPageCount
};

UENUM(BlueprintType)
enum class EMediaRadioPage :uint8
{
	EMediaRadioA1MainPage = 0,
	EMediaRadioA5A7NetNationPage,
	EMediaRadioA6CityTypePage,
	EMediaRadioA8A9ProgramListPage,
	EMediaRadioA12FavoritePage,
	EMediaRadioA13A14RecentlyPage,
	EMediaRadioA17FavoriteDetailPage,
	EMediaRadioPageCount,
	EMediaRadioNoNetWork
};

UENUM(BlueprintType)
enum class EMediaAudioBooksPage :uint8
{
	EMediaAudioBooksA1HomePage = 0,
	EMediaAudioBooksA2ClassifyPage,
	EMediaAudioBooksA3RankingList,
	EMediaAudioBooksA4AlbumPage,
	EMediaAudioBooksA5AnchorPage,
	EMediaAudioBooksA6SearchPage,
	EMediaAudioBooksA11NoNetwork,
	EMediaAudioBooksA13Loading,
	EMediaAudioBooksA14Mine,
	EMediaAudioBooksA15LoginAgreement,
	EMediaAudioBooksA17RecentlyListened,
	EMediaAudioBooksPageCount
};
UENUM(BlueprintType)
enum class EMediaBTPage :uint8
{
	EMediaBTA1MainPage = 0,
	EMediaBTPageCount
};

UENUM(BlueprintType)
enum class EMediaUSBPage :uint8
{
	EMediaUSBA1ConnectState = 0,
	EMediaUSBA4MainPage,
	EMediaUSBA6FolderDetailPage,
	EMediaUSBPageCount
};

UENUM(BlueprintType)
enum class EMediaPopPage :uint8
{
	EMediaPopLocalTuning = 0,
	EMediaPopTuningAuto,
	EMediaPopPageCount
};

UENUM(BlueprintType)
enum class EUsbConnectState : uint8
{
	NotInserted = 0,
	Loading,
	None,
	NoData,
	NowDeviceNoData,
	HaveData,
	HaveMultiData
};

UENUM(BlueprintType)
enum class EAudioAlbumPageType:uint8
{
	HaveDataEdit,
	HaveDataNormal,
	NoData
};

struct FOnlineRadioData
{
	FString RequestId;
	FString AlbumUrl;
	FString AlbumName;
};

struct FAlbumDetail
{
	FString RequestId;
	FString AlbumUrl;
	FString AlbumName;
	FString AlbumNameDate;
};

struct FMusicSongList
{
	FString RequestId;
	FString AlbumUrl;
	FString AlbumName;
	bool IsSub;
};

struct FTwoString
{
	FString OneStr;
	FString TwoStr;
};

struct FAudioBooksA2ClassifyPageParams
{
	bool FromVr = false;
	FString KeyWord;//从语音进入时传入名称，从首页进入时传入ID
};

UENUM(BlueprintType)
enum class EQQMusicQuality : uint8
{
	None = 0,
	Default = 1,//标清
	HQ = 2,		//高清
	SQ = 3,		//超清
	HiRes = 5	//极致
};

DECLARE_LOG_CATEGORY_EXTERN(MediaAppLog, Log, All);