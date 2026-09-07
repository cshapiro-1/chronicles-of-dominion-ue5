#include "DominionTechTreeSubsystem.h"

void UDominionTechTreeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    TechTree.Empty();
    CurrentEpoch = ETechEpoch::EpochI_Bronze;

    // Default Techs
    RegisterTech(FName("BasicAgronomy"), ETechEpoch::EpochI_Bronze, {});
    RegisterTech(FName("BronzeMetallurgy"), ETechEpoch::EpochI_Bronze, {});
    RegisterTech(FName("IronMetallurgy"), ETechEpoch::EpochII_Iron, {FName("BronzeMetallurgy")});
    RegisterTech(FName("AdministrativeLaws"), ETechEpoch::EpochIII_Feudal, {FName("IronMetallurgy")});
    RegisterTech(FName("NavalArmaments"), ETechEpoch::EpochIV_Steam, {FName("AdministrativeLaws")});
}

void UDominionTechTreeSubsystem::Deinitialize()
{
    TechTree.Empty();
    Super::Deinitialize();
}

void UDominionTechTreeSubsystem::RegisterTech(FName TechName, ETechEpoch Epoch, TArray<FName> Prerequisites)
{
    if (!TechTree.Contains(TechName))
    {
        FTechInvention NewTech;
        NewTech.TechName = TechName;
        NewTech.Epoch = Epoch;
        NewTech.Prerequisites = Prerequisites;
        NewTech.bIsUnlocked = false;
        TechTree.Add(TechName, NewTech);
    }
}

bool UDominionTechTreeSubsystem::UnlockTech(FName TechName)
{
    if (FTechInvention* Tech = TechTree.Find(TechName))
    {
        if (Tech->bIsUnlocked) return true;

        if (Tech->Epoch > CurrentEpoch) return false;

        for (const FName& Prereq : Tech->Prerequisites)
        {
            if (FTechInvention* PrereqTech = TechTree.Find(Prereq))
            {
                if (!PrereqTech->bIsUnlocked)
                {
                    return false;
                }
            }
        }

        Tech->bIsUnlocked = true;
        return true;
    }
    return false;
}

bool UDominionTechTreeSubsystem::IsTechUnlocked(FName TechName) const
{
    if (const FTechInvention* Tech = TechTree.Find(TechName))
    {
        return Tech->bIsUnlocked;
    }
    return false;
}

void UDominionTechTreeSubsystem::AdvanceEpoch(ETechEpoch NewEpoch)
{
    if (NewEpoch > CurrentEpoch)
    {
        CurrentEpoch = NewEpoch;
    }
}
