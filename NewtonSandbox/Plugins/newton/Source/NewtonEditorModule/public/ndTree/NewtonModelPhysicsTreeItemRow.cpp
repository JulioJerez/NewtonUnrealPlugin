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


void SNewtonModelPhysicsTreeItemRow::Construct(const FArguments& args, const TSharedRef<STableViewBase>& ownerTableView)
{
	m_item = args._m_item;
	
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
