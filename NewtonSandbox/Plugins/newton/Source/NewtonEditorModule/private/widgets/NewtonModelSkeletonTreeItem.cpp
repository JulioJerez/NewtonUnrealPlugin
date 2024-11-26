// Copyright Epic Games, Inc. All Rights Reserved.

#include "widgets/NewtonModelSkeletonTreeItem.h"


#include "widgets/SSkeletonTreeRow.h"


FNewtonModelSkeletonTreeItem::FNewtonModelSkeletonTreeItem(const TSharedRef<class ISkeletonTree>& InSkeletonTree)
	:SkeletonTreePtr(InSkeletonTree)
	,FilterResult(ESkeletonTreeFilterResult::Shown)
{
	check(0);
}


FName FNewtonModelSkeletonTreeItem::GetRowItemName() const
{
	check(0);
	return FName(TEXT("xxxxxx"));
}

void FNewtonModelSkeletonTreeItem::GenerateWidgetForNameColumn(TSharedPtr< SHorizontalBox > Box, const TAttribute<FText>& FilterText, FIsSelected InIsSelected)
{
	check(0);
}

TSharedRef<ITableRow> FNewtonModelSkeletonTreeItem::MakeTreeRowWidget(const TSharedRef<STableViewBase>& InOwnerTable, const TAttribute<FText>& InFilterText)
{
	check(0);
	return SNew(SSkeletonTreeRow, InOwnerTable)
		.FilterText(InFilterText)
		.Item(SharedThis(this))
		.OnDraggingItem(this, &FNewtonModelSkeletonTreeItem::OnDragDetected);
}

