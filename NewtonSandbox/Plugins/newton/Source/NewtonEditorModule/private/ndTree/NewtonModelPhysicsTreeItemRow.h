// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/BitArray.h"
#include "Delegates/Delegate.h"
#include "Framework/Views/ITypedTableView.h"
#include "HAL/Platform.h"
#include "HAL/PlatformCrt.h"
#include "Input/Reply.h"
#include "Internationalization/Text.h"
#include "Misc/Attribute.h"
#include "Templates/SharedPointer.h"
#include "Templates/UnrealTemplate.h"
#include "UObject/NameTypes.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"

//#include "NewtonModelPhysicsTreeItem.h"

class FDragDropEvent;
class STableViewBase;
class SWidget;
struct FGeometry;
struct FMargin;
struct FPointerEvent;
class FNewtonModelPhysicsTreeItem;

DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnDraggingTreeItem, const FGeometry&, const FPointerEvent&);

class SNewtonModelPhysicsTreeItemRow : public SMultiColumnTableRow<TSharedPtr<FNewtonModelPhysicsTreeItem>>
{
	public:
	SLATE_BEGIN_ARGS(SNewtonModelPhysicsTreeItemRow) 
	{
	}

	// The item for this row 
	SLATE_ARGUMENT(TSharedPtr<FNewtonModelPhysicsTreeItem>, m_item)

	// Filter text typed by the user into the parent tree's search widget 
	//SLATE_ATTRIBUTE(FText, FilterText);

	// Delegate for dragging items
	//SLATE_EVENT(FOnDraggingTreeItem, OnDraggingItem);

	SLATE_END_ARGS()

	void Construct(const FArguments& args, const TSharedRef<STableViewBase>& ownerTableView);

	// Overridden from SMultiColumnTableRow.  Generates a widget for this column of the tree row.
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

	private:
	// The item this row is holding
	TWeakPtr<FNewtonModelPhysicsTreeItem> m_item;
};
