// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/CPInteractiveObjectInterface.h"

// Add default functionality here for any ICPInteractiveObjectInterface functions that are not pure virtual.

void ICPInteractiveObjectInterface::HighlightObjectOutline()
{
	// Highlight the outline of object Mesh
	UStaticMeshComponent* MeshComponent = GetMeshComponent();
	MeshComponent->SetRenderCustomDepth(true);
	MeshComponent->SetCustomDepthStencilValue(2);
}

void ICPInteractiveObjectInterface::UnhighlightObjectOutline()
{
	// Unhighlight the outline of object mesh
	UStaticMeshComponent* MeshComponent = GetMeshComponent();
	MeshComponent->SetRenderCustomDepth(false);
	MeshComponent->SetCustomDepthStencilValue(0);
}
