

button = '''
Begin Object Class=/Script/BlueprintGraph.K2Node_ComponentBoundEvent Name="K2Node_ComponentBoundEvent_{i1}"
   DelegatePropertyName="OnClicked"
   DelegateOwnerClass=Class'"/Script/UMG.Button"'
   ComponentPropertyName="{Name}"
   DelegatePropertyDisplayName=NSLOCTEXT("UObjectDisplayNames", "Button:OnClicked", "On Clicked")
   EventReference=(MemberParent=Package'"/Script/UMG"',MemberName="OnButtonClickedEvent__DelegateSignature")
   bInternalEvent=True
   CustomFunctionName="BndEvt__{Class}_{Name}_K2Node_ComponentBoundEvent_{i1}_OnButtonClickedEvent__DelegateSignature"
   NodePosX=0
   NodePosY={y1}
   NodeGuid=6BD79F814B4E8A0EE44F869FC3C1D5CE
   CustomProperties Pin (PinId=E8F0301D45674BE6C33CB29F0A3F62AC,PinName="OutputDelegate",Direction="EGPD_Output",PinType.PinCategory="delegate",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(MemberParent=WidgetBlueprintGeneratedClass'"/Game/NIC/Core/Widgets/Multi-Media/SubWidgets/{Class}.{Class}_C"',MemberName="BndEvt__{Class}_{Name}_K2Node_ComponentBoundEvent_{i1}_OnButtonClickedEvent__DelegateSignature"),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
   CustomProperties Pin (PinId=12210B8244E1E8FEA6655BB6A3FC8748,PinName="then",Direction="EGPD_Output",PinType.PinCategory="exec",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,LinkedTo=(K2Node_CallFunction_22 D6EBBB1B45BEFB2BE3AADB916E134133,),PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
End Object

Begin Object Class=/Script/BlueprintGraph.K2Node_ComponentBoundEvent Name="K2Node_ComponentBoundEvent_{i2}"
   DelegatePropertyName="OnReleased"
   DelegateOwnerClass=Class'"/Script/UMG.Button"'
   ComponentPropertyName="{Name}"
   DelegatePropertyDisplayName=NSLOCTEXT("UObjectDisplayNames", "Button:OnReleased", "On Released")
   EventReference=(MemberParent=Package'"/Script/UMG"',MemberName="OnButtonReleasedEvent__DelegateSignature")
   bInternalEvent=True
   CustomFunctionName="BndEvt__{Class}_{Name}_K2Node_ComponentBoundEvent_{i2}_OnButtonReleasedEvent__DelegateSignature"
   NodePosX=0
   NodePosY={y2}
   NodeGuid=C6AE958A44F7FF46040C65BB1D6F3F86
   CustomProperties Pin (PinId=FB8BF9F94A75EB77D71339A44A869D22,PinName="OutputDelegate",Direction="EGPD_Output",PinType.PinCategory="delegate",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(MemberParent=WidgetBlueprintGeneratedClass'"/Game/NIC/Core/Widgets/Multi-Media/SubWidgets/{Class}.{Class}_C"',MemberName="BndEvt__{Class}_{Name}_K2Node_ComponentBoundEvent_{i2}_OnButtonReleasedEvent__DelegateSignature"),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
   CustomProperties Pin (PinId=CD46D6B3473489AD3643DBBEA6A4AFAC,PinName="then",Direction="EGPD_Output",PinType.PinCategory="exec",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,LinkedTo=(K2Node_CallFunction_24 D6EBBB1B45BEFB2BE3AADB916E134133,),PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
End Object

'''

event = '''
Begin Object Class=/Script/BlueprintGraph.K2Node_Event Name="K2Node_Event_13"
   EventReference=(MemberParent=Class'"/Script/NIC.{Class}"',MemberName="{Name}")
   bOverrideFunction=True
   NodePosX=-16
   NodePosY=1680
   NodeGuid=7203167F4683954FD16A2D8ED367E2C4
   CustomProperties Pin (PinId=9C02CC054735560E8D513F93AB4A5582,PinName="OutputDelegate",Direction="EGPD_Output",PinType.PinCategory="delegate",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(MemberParent=Class'"/Script/NIC.MiniPlayer"',MemberName="SetCurrentSource"),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
   CustomProperties Pin (PinId=BF1F2DFE4927C09AE1E13796E6AC2AF4,PinName="then",Direction="EGPD_Output",PinType.PinCategory="exec",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
   CustomProperties Pin (PinId=A95B2D754C6F7B585269B2AEEA6A4B20,PinName="SourceType",PinToolTip="Source Type\nInteger",Direction="EGPD_Output",PinType.PinCategory="int",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,DefaultValue="0",AutogeneratedDefaultValue="0",PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
End Object

Begin Object Class=/Script/BlueprintGraph.K2Node_Event Name="K2Node_Event_0"
   EventReference=(MemberParent=Class'"/Script/NIC.MiniPlayer"',MemberName="SetAlbum")
   bOverrideFunction=True
   NodePosY=1312
   NodeGuid=31A3BD7646FDE71823CF29B4377B6BBB
   CustomProperties Pin (PinId=38DE0DD543E278C211A97895B762C9EA,PinName="OutputDelegate",Direction="EGPD_Output",PinType.PinCategory="delegate",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(MemberParent=Class'"/Script/NIC.MiniPlayer"',MemberName="SetAlbum"),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
   CustomProperties Pin (PinId=BB4B5F6940908B3F2FD054A20B133E69,PinName="then",Direction="EGPD_Output",PinType.PinCategory="exec",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
   CustomProperties Pin (PinId=FAAA26CC4711015B2C33709442C55E81,PinName="Url",PinToolTip="Url\nString",Direction="EGPD_Output",PinType.PinCategory="string",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
End Object

'''

print(button.format(i1=1,i2=2,Name='Button_31',y1=0,y2=200,Class='WBP_MP_Button_232X72'))

