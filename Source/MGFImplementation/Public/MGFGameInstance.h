// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "MGFGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPatchCompleteDelegate, bool, Succeeded);

UCLASS()
class MGFIMPLEMENTATION_API UMGFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
	virtual void Shutdown() override;

	/** Starts the game patching process. Returns false if the patching manifest is not up to date. */
	UFUNCTION(BlueprintCallable, Category = "Patching")
	bool PatchGame();

	UFUNCTION(BlueprintCallable, Category = "Patching")
	void InitPatching();

	/** Fired when the patching manifest has been queried and we're ready to decide whether to patch the game or not*/
	UPROPERTY(BlueprintAssignable, Category = "Patching");
	FPatchCompleteDelegate OnPatchReady;

	void OnPatchVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:
	//Tracks Whether or not our local manifest file is up to date with the one hosted on our website
	bool bIsDownloadManifestUpToDate = false;

	bool bIsPatchingGame = false;

	UPROPERTY(EditDefaultsOnly, Category = "Patching")
	FString PatchVersionURL;

	UPROPERTY(EditDefaultsOnly, Category = "Patching")
	TArray<int32> ChunkDownloadList;

	void OnManifestUpdateComplete(bool bSuccess);

	UFUNCTION(BlueprintPure, Category = "Patching|Stats")
	void GetLoadingProgress(int32& FilesDownloaded, int32& TotalFilesToDownload, float& DownloadPercent, int32& ChunksMounted, int32& TotalChunksToMount, float& MountPercent) const;

	/** Fired when the patching process succeeds or fails */
	UPROPERTY(BlueprintAssignable, Category = "Patching");
	FPatchCompleteDelegate OnPatchComplete;

	/** Called when the chunk download process finishes */
	void OnDownloadComplete(bool bSuccess);

	/** Called whenever ChunkDownloader's loading mode is finished*/
	void OnLoadingModeComplete(bool bSuccess);

	/** Called when ChunkDownloader finishes mounting chunks */
	void OnMountComplete(bool bSuccess);
	
};
