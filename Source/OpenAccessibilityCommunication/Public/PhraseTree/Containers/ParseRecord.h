// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Input/UParseInput.h"
#include "PhraseTree/Containers/ContextObject.h"

#include "ParseRecord.generated.h"

/// <summary>
/// The Collected Information from the Propogation of the Phrase through the tree.
/// </summary>
USTRUCT(BlueprintType)
struct OPENACCESSIBILITYCOMMUNICATION_API FParseRecord
{
	GENERATED_BODY()

public:
	friend class FPhraseTree;

	FParseRecord()
	{
		PhraseInputs = TMultiMap<FString, UParseInput*>();
		ContextObjectStack = TArray<UPhraseTreeContextObject*>();
	}

	FParseRecord(TArray<UPhraseTreeContextObject*> InContextObjects)
	{
		PhraseInputs = TMultiMap<FString, UParseInput*>();
		ContextObjectStack = InContextObjects;
	}

	~FParseRecord()
	{
		PhraseInputs.Empty();
	}

	// -- Phrase String

	/// <summary>
	/// Gets the Recorded Phrase String for this record of propagation.
	/// </summary>
	/// <returns></returns>
	const FString GetPhraseString() const
	{
		return PhraseString;
	}

	void AddPhraseString(FString StringToRecord)
	{
		PhraseString += TEXT(" ") + StringToRecord;
	}

	// --


	/// <summary>
	/// Gets the Input for the Provided Phrase, if it exists.
	/// </summary>
	/// <param name="InString">- The Phrase To Check For An Input.</param>
	/// <returns>The Found PhraseInput For the Phrase, otherwise nullptr.</returns>
	UParseInput* GetPhraseInput(const FString& InString)
	{
		return *PhraseInputs.Find(InString);
	}

	/// <summary>
	/// Gets the Input for the Provided Phrase, if it exists.
	/// </summary>
	/// <typeparam name="CastToType">DownCast Type For the Phrase Input (Must Derrive From UPhraseInput).</typeparam>
	/// <param name="InString">- The Phrase To Check For An Input.</param>
	/// <returns>The Found DownCasted PhraseInput, otherwise nullptr.</returns>
	template<class CastToType>
	CastToType* GetPhraseInput(const FString& InString)
	{
		// Check If The Phrase Exits
		// This Error Will Be Thrown If: InString Is In Correct (Requires UpperCase) or The Phrase Does Not Exist.
		check(PhraseInputs.Contains(InString))

		return Cast<CastToType>(*PhraseInputs.Find(InString));
	}

	/// <summary>
	/// Gets the Input for the Provided Phrase, if it exists.
	/// </summary>
	/// <param name="InString">- The Phrase To Check For An Input.</param>
	/// <param name="OutInput">- Returns the Found Input or nullptr.</param>
	void GetPhraseInput(const FString& InString, UParseInput* OutInput)
	{
		// Check If The Phrase Exits
		// This Error Will Be Thrown If: InString Is In Correct (Requires UpperCase) or The Phrase Does Not Exist.
		check(PhraseInputs.Contains(InString))

		OutInput = *PhraseInputs.Find(InString);
	}

	/// <summary>
	/// Gets the Input for the Porvided Phrase, if it exists.
	/// </summary>
	/// <typeparam name="CastToType">DownCast Type For the Phrase Input (Must Derrive From UPhraseInput).</typeparam>
	/// <param name="InString">- The Phrase To Check For An Input.</param>
	/// <param name="OutInput">- Returns the Found DownCasted Input or nullptr.</param>
	template<class CastToType>
	void GetPhraseInput(const FString& InString, CastToType* OutInput)
	{
		// Check If The Phrase Exits
		// This Error Will Be Thrown If: InString Is In Correct (Requires UpperCase) or The Phrase Does Not Exist.
		check(PhraseInputs.Contains(InString))

		OutInput = Cast<CastToType>(*PhraseInputs.Find(InString));
	}

	// -- GetPhraseInputs

	/// <summary>
	/// Gets an Array of Phrase Inputs for the Provided Phrase.
	/// </summary>
	/// <param name="InString">- The Phrase To Check For A Multi-Input.</param>
	/// <param name="OutInputs">- Returns An Array of Inputs.</param>
	/// <param name="MaintainOrder">- Should the Returned Array Maintain the Order the Inputs where Inserted.</param>
	void GetPhraseInputs(const FString& InString, TArray<UParseInput*>& OutInputs, const bool MaintainOrder = true)
	{
		// Check If The Phrase Exits
		// This Error Will Be Thrown If: InString Is In Correct (Requires UpperCase) or The Phrase Does Not Exist.
		check(PhraseInputs.Contains(InString))

		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);
	}

	/// <summary>
	/// Gets an Array of Phrase Inputs for the Provided Phrase.
	/// </summary>
	/// <param name="InString">- The Phrase To Check For A Multi-Input.</param>
	/// <param name="MaintainOrder">- Should the Returned Array Maintain the Order the Inputs where Inserted.</param>
	/// <returns>The Array of Found Inputs.</returns>
	TArray<UParseInput*> GetPhraseInputs(const FString& InString, const bool MaintainOrder = true)
	{
		// Check If The Phrase Exits
		// This Error Will Be Thrown If: InString Is In Correct (Requires UpperCase) or The Phrase Does Not Exist.
		check(PhraseInputs.Contains(InString))

		TArray<UParseInput*> OutInputs;

		PhraseInputs.MultiFind(InString, OutInputs, MaintainOrder);

		return OutInputs;
	}

	// -- PhraseInput

	/// <summary>
	/// Adds a Phrase Input to the Record.
	/// </summary>
	/// <param name="InString">- The Phrase to Bind the Input To.</param>
	/// <param name="InInput">- The Phrase Input Object Containing Input Data.</param>
	void AddPhraseInput(const FString& InString, UParseInput* InInput)
	{
		PhraseInputs.Add(InString.ToUpper(), InInput);
	}

	/// <summary>
	/// Removes a Phrase Input From The Record.
	/// </summary>
	/// <param name="InString">- The Phrase To Remove All Bound Inputs from.</param>
	void RemovePhraseInput(const FString& InString)
	{
		PhraseInputs.Remove(InString);
	}

	// -- ContextObject Stack Modification

	/// <summary>
	/// Pushes a Context Object onto the Stack.
	/// </summary>
	/// <param name="InObject">- The Context Object To Push onto The Stack.</param>
	void PushContextObj(UPhraseTreeContextObject* InObject)
	{
		this->ContextObjectStack.Push(InObject);
	}

	/// <summary>
	/// Pops the Top Context Object From The Stack.
	/// </summary>
	void PopContextObj()
	{
		this->ContextObjectStack.Pop();
	}

	/// <summary>
	/// Pops the Top Context Object From The Stack.
	/// </summary>
	/// <param name="OutObject">- The Popped Context Object.</param>
	void PopContextObj(UPhraseTreeContextObject* OutObject)
	{
		OutObject = this->ContextObjectStack.Pop();
	}

	/// <summary>
	/// Removes a Select Context Object From The Stack.
	/// </summary>
	/// <param name="InObject"></param>
	void RemoveContextObj(UPhraseTreeContextObject* InObject)
	{
		this->ContextObjectStack.Remove(InObject);
	}

	// -- HasContextObj
	
	/// <summary>
	/// Checks if there is a Context Object on the Stack.
	/// </summary>
	/// <returns></returns>
	bool HasContextObj()
	{
		return this->ContextObjectStack.Num() > 0;
	}

	/// <summary>
	/// Checks if a specific Context Object is on the Stack.
	/// </summary>
	/// <param name="InObject">- The Context Object To Check if On The Stack.</param>
	/// <returns>True, if the Object is on the Stack. False, if the Object is not on the stack.</returns>
	bool HasContextObj(UPhraseTreeContextObject* InObject)
	{
		return HasContextObj() && this->ContextObjectStack.Contains(InObject);
	}

	// -- GetContextObj

	/// <summary>
	/// Gets the Top Context Object On The Stack.
	/// </summary>
	/// <returns>The Top Context Object On The Stack.</returns>
	UPhraseTreeContextObject* GetContextObj()
	{
		return this->ContextObjectStack.Last();
	}

	/// <summary>
	/// Gets the Top Context Object On The Stack.
	/// </summary>
	/// <param name="OutObject">- Returns the Top Context Object On The Stack.</param>
	void GetContextObj(UPhraseTreeContextObject* OutObject)
	{
		OutObject = this->ContextObjectStack.Last();
	}

	/// <summary>
	/// Gets the Top Context Object On The Stack.
	/// </summary>
	/// <typeparam name="CastToType">DownCast Type For the Context Object (Must Derrive From UPhraseTreeContextObject).</typeparam>
	/// <returns>The DownCasted Context Object, otherwise nullptr.</returns>
	template<class CastToType>
	CastToType* GetContextObj()
	{
		return Cast<CastToType>(this->ContextObjectStack.Last());
	}

	/// <summary>
	/// Gets the Top Context Object On The Stack.
	/// </summary>
	/// <typeparam name="CastToType">DownCast Type For the Context Object (Must Derrive From UPhraseTreeContextObject).</typeparam>
	/// <param name="OutObject">- Returns the Downcasted Context Object, otherwise nullptr.</param>
	template<class CastToType>
	void GetContextObj(CastToType* OutObject)
	{
		OutObject = Cast<CastToType>(this->ContextObjectStack.Last());
	}

	/// <summary>
	/// Gets the Entire Context Stack.
	/// </summary>
	/// <param name="OutContextStack">- Returns the Current Context Stack.</param>
	void GetContextStack(TArray<UPhraseTreeContextObject*> OutContextStack)
	{
		OutContextStack = ContextObjectStack;
	}

	/// <summary>
	/// Gets the Entire Context Stack.
	/// </summary>
	/// <returns>The Current Context Stack.</returns>
	TArray<UPhraseTreeContextObject*> GetContextStack()
	{
		return ContextObjectStack;
	}

protected:

	/// <summary>
	/// The Context Stack of Context Objects.
	/// </summary>
	TArray<UPhraseTreeContextObject*> ContextObjectStack = TArray<UPhraseTreeContextObject*>();

	/// <summary>
	/// A Record of the Phrase String used through-out propagation.
	/// </summary>
	FString PhraseString;

	/// <summary>
	/// Map of all the Provided Phrase Inputs, to their Respective Phrases.
	/// </summary>
	TMultiMap<FString, UParseInput*> PhraseInputs;

};