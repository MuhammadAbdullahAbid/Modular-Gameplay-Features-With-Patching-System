// Fill out your copyright notice in the Description page of Project Settings.


#include "MGFGameInstance.h"
#include "ChunkDownloader.h"
#include "Misc/CoreDelegates.h"
#include "AssetRegistryModule.h"

void UMGFGameInstance::Init()
{
    Super::Init();

    // deployment name to use for this patcher configuration
    const FString DeploymentName = "PatchingTestLive";

    // content build ID. Our HTTP response will provide this info
    // so we can update to new patch versions as they become available on the server
    const FString ContentBuildId = "PatchingTestKey";

    UE_LOG(LogTemp, Warning, TEXT("Patch Content ID Response: %s"), *ContentBuildId);

    // ---------------------------------------------------------

    UE_LOG(LogTemp, Warning, TEXT("Getting Chunk Downloader"));

    //initialize the chunk downloader with the chosen platform
    TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetOrCreate();
    Downloader->Initialize("Windows", 8);

    UE_LOG(LogTemp, Warning, TEXT("Loading Cached Build ID"));

    // Try to load the cached build manifest data if it exists.
    //This will populate the Chunk Downloader state with the most recently downloaded manifest data.
    if (Downloader->LoadCachedBuild(DeploymentName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cached Build Succeeded"));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Cached Build Failed"));
    }

    UE_LOG(LogTemp, Warning, TEXT("Updating Build Manifest"));

    // Check with the server if there's a newer manifest file that matches our data, and download it.
    // ChunkDownloader uses a Lambda function to let us know when the async donwload is complete.
    TFunction<void(bool)> ManifestCompleteCallback = [this](bool bSuccess)
    {
        // write to the log
        if (bSuccess)
        {
            UE_LOG(LogTemp, Warning, TEXT("Manifest Update Succeeded"));
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Manifest Update Failed"));
        }

        bIsDownloadManifestUpToDate = bSuccess;
        // call our delegate to let the game know we're ready to start patching
        OnPatchReady.Broadcast(bSuccess);
    };

    //// Start the manifest update process, passing the BuildID and Lambda callback
    Downloader->UpdateBuild(DeploymentName, ContentBuildId, ManifestCompleteCallback);
}

void UMGFGameInstance::Shutdown()
{
    Super::Shutdown();

    // Shut down ChunkDownloader
     FChunkDownloader::Shutdown();
}

bool UMGFGameInstance::PatchGame()
{
    // Make sure the download manifest is up to date
    if (!bIsDownloadManifestUpToDate)
    {
        // we couldn't contact the server to validate our manifest, so we can't patch
        UE_LOG(LogTemp, Display, TEXT("Manifest Update Failed. Can't patch the game"));

        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("Starting the game patch process."));

    // get the chunk downloader
    TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();

    // raise the patching flag
    bIsPatchingGame = true;

    // Write the current status for each chunk file from the manifest.
    // This way we can find out if a specific chunk is already downloaded, in progress, pending, etc.
    for (int32 ChunkID : ChunkDownloadList)
    {
        // print the chunk status to the log for debug purposes. We can do more complex logic here if we want.
        int32 ChunkStatus = static_cast<int32>(Downloader->GetChunkStatus(ChunkID));
        UE_LOG(LogTemp, Display, TEXT("Chunk %i status: %i"), ChunkID, ChunkStatus);
    }

    // prepare the pak mounting callback lambda function
    TFunction<void(bool)> MountCompleteCallback = [this](bool bSuccess)
    {
        // lower the chunk mount flag
        bIsPatchingGame = false;

        if (bSuccess)
        {
            UE_LOG(LogTemp, Display, TEXT("Chunk Mount complete"));

            // call the delegate
            OnPatchComplete.Broadcast(true);
        }
        else {
            UE_LOG(LogTemp, Display, TEXT("Chunk Mount failed"));

            // call the delegate
            OnPatchComplete.Broadcast(false);
        }
    };

    // mount the Paks so their assets can be accessed
    Downloader->MountChunks(ChunkDownloadList, MountCompleteCallback);

   
    //// make sure the download manifest is up to date
    //if (bIsDownloadManifestUpToDate)
    //{
    //    // get the chunk downloader
    //    TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();

    //    // report current chunk status
    //    for (int32 ChunkID : ChunkDownloadList)
    //    {
    //        int32 ChunkStatus = static_cast<int32>(Downloader->GetChunkStatus(ChunkID));
    //        UE_LOG(LogTemp, Display, TEXT("Chunk %i status: %i"), ChunkID, ChunkStatus);
    //    }

    //    TFunction<void(bool bSuccess)> DownloadCompleteCallback = [&](bool bSuccess) {OnDownloadComplete(bSuccess); };
    //    Downloader->DownloadChunks(ChunkDownloadList, DownloadCompleteCallback, 1);

    //    // start loading mode
    //    TFunction<void(bool bSuccess)> LoadingModeCompleteCallback = [&](bool bSuccess) {OnLoadingModeComplete(bSuccess); };
    //    Downloader->BeginLoadingMode(LoadingModeCompleteCallback);
    //    return true;
    //}

    //// we couldn't contact the server to validate our manifest, so we can't patch
    //UE_LOG(LogTemp, Display, TEXT("Manifest Update Failed. Can't patch the game"));

    return true;
}

void UMGFGameInstance::InitPatching()
{
}

void UMGFGameInstance::OnPatchVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
}

void UMGFGameInstance::OnManifestUpdateComplete(bool bSuccess)
{
    bIsDownloadManifestUpToDate = bSuccess;
}

void UMGFGameInstance::GetLoadingProgress(int32& FilesDownloaded, int32& TotalFilesToDownload, float& DownloadPercent, int32& ChunksMounted, int32& TotalChunksToMount, float& MountPercent) const
{
    //Get a reference to ChunkDownloader
    TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();

    //Get the loading stats struct
    FChunkDownloader::FStats LoadingStats = Downloader->GetLoadingStats();

    //Get the bytes downloaded and bytes to download
    FilesDownloaded = LoadingStats.BytesDownloaded;
    TotalFilesToDownload = LoadingStats.TotalBytesToDownload;

    //Get the number of chunks mounted and chunks to download
    ChunksMounted = LoadingStats.ChunksMounted;
    TotalChunksToMount = LoadingStats.TotalChunksToMount;

    //Calculate the download and mount percent using the above stats
    DownloadPercent = (float)FilesDownloaded / (float)TotalFilesToDownload;
    MountPercent = (float)ChunksMounted / (float)TotalChunksToMount;
}

void UMGFGameInstance::OnDownloadComplete(bool bSuccess)
{
    // was Pak download successful?
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Display, TEXT("Patch Download Failed"));

        // call the delegate
        OnPatchComplete.Broadcast(false);

        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Patch Download Complete"));

    // get the chunk downloader
    TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();

    // build the downloaded chunk list
    FJsonSerializableArrayInt DownloadedChunks;

    for (int32 ChunkID : ChunkDownloadList)
    {
        DownloadedChunks.Add(ChunkID);
    }

    // prepare the pak mounting callback lambda function
    TFunction<void(bool)> MountCompleteCallback = [this](bool bSuccess)
    {
        // lower the chunk mount flag
        bIsPatchingGame = false;

        if (bSuccess)
        {
            UE_LOG(LogTemp, Display, TEXT("Chunk Mount complete"));

            // call the delegate
            OnPatchComplete.Broadcast(true);
        }
        else {
            UE_LOG(LogTemp, Display, TEXT("Chunk Mount failed"));

            // call the delegate
            OnPatchComplete.Broadcast(false);
        }
    };
    OnMountComplete(bSuccess);
    // mount the Paks so their assets can be accessed
    Downloader->MountChunks(DownloadedChunks, MountCompleteCallback);
}

void UMGFGameInstance::OnLoadingModeComplete(bool bSuccess)
{
    OnDownloadComplete(bSuccess);
}

void UMGFGameInstance::OnMountComplete(bool bSuccess)
{
    OnPatchComplete.Broadcast(bSuccess);
}
