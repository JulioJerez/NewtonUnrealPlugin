/* Copyright (c) <2024-2024> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/


#include "ndTree/NewtonModelPhysicsTreeItemRow.h"

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
