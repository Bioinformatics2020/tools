#pragma once

UENUM(BlueprintType)
enum class EMediaEditType : uint8
{
	ChooseNull,
	ChooseNormal,
	ChooseAll
};

class IChooseComment;
class MediaDeleteManager;
DECLARE_DELEGATE_OneParam(FMediaEditTypeDelegate, EMediaEditType);
DECLARE_DELEGATE_OneParam(FIntValueChanged, int);

/**
 * @brief 编辑删除管理器 子项需要继承IChooseComment接口。
 * 提供了被选中元素的数量，选择的状态全选、全不选、正常，添加子项、移除子项、全部移除
 * 可以在子项中更方便的修改自己的状态，并直接同步状态到管理器中；在管理器中可以修改子项状态，并同步到子项中。
 * 在编辑这一个功能点上，完成了子项与父项的解耦。
 */
class DeleteManager
{
public:
	virtual ~DeleteManager() = default;
	//提供了全选状态的回调
	void Init(TFunction<void(bool)> ChooseAll);
	//补充提供全选与全不选
	FMediaEditTypeDelegate OnMediaEditTypeChange;
	FIntValueChanged OnNumberChange;

	/**
	 * @brief 全选后，遍历设置控件选中态样式，并把控件id传入删除数组
	 * @param InChoose 
	 */
	virtual void ChooseAll(bool InChoose);
	
	virtual void ChangePlayer(IChooseComment* ChooseComment);

	void AddPlayer(IChooseComment* ChooseComment, bool InbChoosed = false);
	void AddPlayers(TArray<IChooseComment*> ChooseComments, bool InbChoosed = false);

	void RemovePlayer(IChooseComment* ChooseComment);

	void ClearPlayer();
	
	int GetChooseNum();

	//获取当前选择状态
	EMediaEditType GetMediaEditType();

	//获取所有子项
	TMap<IChooseComment*,bool>& GetAllChooseItem();

protected:
	void CheckChooseMode();
	
	static DeleteManager* UIDeleteManger;

	//所有的被管理元素
	TMap<IChooseComment*,bool> AllChooseItem;
	
	int ChoosedNum = 0;

	//设置全选的样式
	TFunction<void(bool)> ChooseAllStyle;

	//记录当前选择状态
	EMediaEditType MediaEditType = EMediaEditType::ChooseNull;
};


class IChooseComment
{
protected:
	//true 表示已选中，false 表示未选中
	bool ChooseMode = false;

	DeleteManager* mDeleteManager = nullptr;
public:
	virtual ~IChooseComment(){};
	/**
	 * @brief 改变选中状态，或者在子类改变样式
	 * @param bChoosed 
	 */
	virtual void SetChooseMode(bool bChoosed)
	{
		ChooseMode = bChoosed;
	};
	
	void initChooseComment(DeleteManager* InDeleteManager)
	{
		mDeleteManager = InDeleteManager;
	}

	void ChangePlayer()
	{
		if(mDeleteManager)
		{
			mDeleteManager->ChangePlayer(this);
		}
	}
	
	bool GetChooseMode()
	{
		return ChooseMode;
	}
};

/**
 * @brief 相比父类基础的编辑删除功能，额外提供了获取被选中子项ID、进入编辑模式的功能
 */
class MediaDeleteManager:public DeleteManager
{
public:
	TArray<FString> GetChooseId();

	/**
	 * @brief 全选后，遍历设置控件选中态样式，并把控件id传入删除数组
	 * @param InChoose 
	 */
	virtual void ChooseAll(bool InChoose) override;

	virtual void ChangePlayer(IChooseComment* ChooseComment) override;

	//通知所有子控件进入编辑模式
	void EntryEditMode(bool InEditMode,bool DefaultChooseMode = false);
protected:
	TSet<FString> Ids;
};

class IMediaChooseComment:public IChooseComment
{
public:
	void initMediaChooseComment(DeleteManager* InDeleteManager,FString InId)
	{
		initChooseComment(InDeleteManager);
		Id = InId;
	}

	FString GetMediaChooseId()
	{
		return Id;
	}

	virtual void EntryEditMode(bool InEditMode){};

protected:
	FString Id;
};

