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

class SWidget;
struct FMargin;
struct FGeometry;
struct FPointerEvent;
class FDragDropEvent;
class STableViewBase;
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
