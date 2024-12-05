// Copyright Epic Games, Inc. All Rights Reserved.

#include "ndTree/NewtonModelPhysicsTreeItemRow.h"

//#include "Framework/SlateDelegates.h"
//#include "ISkeletonTree.h"
//#include "Layout/Children.h"
//#include "Layout/Margin.h"
//#include "Layout/Visibility.h"
//#include "Misc/AssertionMacros.h"
//#include "Preferences/PersonaOptions.h"
//#include "SlotBase.h"
//#include "UObject/UObjectGlobals.h"
//#include "Widgets/SBoxPanel.h"
//#include "Widgets/SCompoundWidget.h"
//#include "Widgets/Views/SExpanderArrow.h"
//#include "Widgets/Views/SListView.h"
//#include "Widgets/Views/STableViewBase.h"

#include "ndTree/NewtonModelPhysicsTreeItem.h"

//class SWidget;
//struct FGeometry;
//struct FPointerEvent;
//class FDragDropEvent;
//class STableViewBase;

#if 0
void SNewtonModelPhysicsTreeItemRow::ConstructChildren(ETableViewMode::Type ownerTableMode, const TAttribute<FMargin>& padding, const TSharedRef<SWidget>& content)
{
	STableRow<TSharedPtr<FNewtonModelPhysicsTreeItem>>::Content = content;

	TSharedRef<SWidget> InlineEditWidget = Item.Pin()->GenerateInlineEditWidget(FilterText, FIsSelected::CreateSP(this, &STableRow::IsSelected));

	// MultiColumnRows let the user decide which column should contain the expander/indenter item.
	this->ChildSlot
		.Padding(padding)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				content
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				InlineEditWidget
			]
		];
}


void SNewtonModelPhysicsTreeItemRow::OnDragEnter( const FGeometry& geometry, const FDragDropEvent& dragDropEvent )
{
	check(0);
	//Item.Pin()->HandleDragEnter(dragDropEvent);
}

void SNewtonModelPhysicsTreeItemRow::OnDragLeave( const FDragDropEvent& dragDropEvent )
{
	check(0);
	//Item.Pin()->HandleDragLeave(dragDropEvent);
}

FReply SNewtonModelPhysicsTreeItemRow::OnDrop( const FGeometry& geometry, const FDragDropEvent& dragDropEvent )
{
	check(0);
	return FReply;
	//return Item.Pin()->HandleDrop(dragDropEvent);
}

FReply SNewtonModelPhysicsTreeItemRow::OnDragDetected( const FGeometry& geometry, const FPointerEvent& mouseEvent )
{
	if ( OnDraggingItem.IsBound() )
	{
		return OnDraggingItem.Execute( geometry, mouseEvent );
	}
	else
	{
		return FReply::Unhandled();
	}
}

int32 SNewtonModelPhysicsTreeItemRow::DoesItemHaveChildren() const
{
	check(0);
	//if(Item.Pin()->HasInlineEditor())
	//{
	//	return 1;
	//}

	return SMultiColumnTableRow<TSharedPtr<FNewtonModelPhysicsTreeItem>>::DoesItemHaveChildren();
}

bool SNewtonModelPhysicsTreeItemRow::IsItemExpanded() const
{
	check(0);
	return true;
	//return SMultiColumnTableRow<TSharedPtr<FNewtonModelPhysicsTreeItem>>::IsItemExpanded() || Item.Pin()->IsInlineEditorExpanded();
}

void SNewtonModelPhysicsTreeItemRow::ToggleExpansion()
{
	check(0);
	//SMultiColumnTableRow<TSharedPtr<FNewtonModelPhysicsTreeItem>>::ToggleExpansion();
	//
	//if (Item.Pin()->HasInlineEditor())
	//{
	//	Item.Pin()->ToggleInlineEditorExpansion();
	//	OwnerTablePtr.Pin()->Private_SetItemExpansion(Item.Pin(), Item.Pin()->IsInlineEditorExpanded());
	//}
}

#endif


void SNewtonModelPhysicsTreeItemRow::Construct(const FArguments& args, const TSharedRef<STableViewBase>& ownerTableView)
{
	m_item = args._m_item;
	//OnDraggingItem = args._OnDraggingItem;
	//FilterText = args._FilterText;
	
	check( m_item.IsValid() );
	
	const FSuperRowType::FArguments baseArgs = FSuperRowType::FArguments()
		.Style( FAppStyle::Get(), "TableView.AlternatingRow" );
	
	SMultiColumnTableRow< TSharedPtr<FNewtonModelPhysicsTreeItem> >::Construct( baseArgs, ownerTableView );
}

TSharedRef< SWidget > SNewtonModelPhysicsTreeItemRow::GenerateWidgetForColumn(const FName& columnName)
{
	TSharedPtr<FNewtonModelPhysicsTreeItem> item(m_item.Pin());
#if 0
	return SNew(STextBlock)
		.Text(FText::FromName(item->GetDisplayName()))
		.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 10))
		.ColorAndOpacity(FLinearColor(0.8, .8, .8, 1))
		.ShadowColorAndOpacity(FLinearColor::Black)
		.ShadowOffset(FIntPoint(-2, 2));
#else
	TSharedPtr< SHorizontalBox > rowBox;
	SAssignNew(rowBox, SHorizontalBox)
	.Visibility_Lambda([this]()
	{
		//return Item.Pin()->GetFilterResult() == ESkeletonTreeFilterResult::ShownDescendant && GetMutableDefault<UPersonaOptions>()->bHideParentsWhenFiltering ? EVisibility::Collapsed : EVisibility::Visible;
		return EVisibility::Visible;
	});
	
	rowBox->AddSlot()
	.AutoWidth()
	[
		SNew(SExpanderArrow, SharedThis(this))
		.ShouldDrawWires(true)
	];
	//item->GenerateWidgetForNameColumn(rowBox, FilterText, FIsSelected::CreateSP(this, &STableRow::IsSelectedExclusively));
	item->GenerateWidgetForNameColumn(rowBox, FIsSelected::CreateSP(this, &STableRow::IsSelectedExclusively));

	return rowBox.ToSharedRef();
#endif
}
