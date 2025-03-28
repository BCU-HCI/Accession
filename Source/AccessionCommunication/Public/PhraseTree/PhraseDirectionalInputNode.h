#pragma once

#include "CoreMinimal.h"

#include "PhraseEnumInputNode.h"
#include "Containers/Input/InputContainers.h"

class ACCESSIONCOMMUNICATION_API FPhraseDirectionalInputNode : public FPhraseEnumInputNode<EPhraseDirectionalInput>
{
public:
	FPhraseDirectionalInputNode(const TCHAR *NodeName)
		: FPhraseEnumInputNode<EPhraseDirectionalInput>(NodeName)
	{
	}

	FPhraseDirectionalInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhraseDirectionalInput>(NodeName, InChildNodes)
	{
	}

	FPhraseDirectionalInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhraseDirectionalInput>(NodeName, InOnPhraseParsed, InChildNodes)
	{
	}

	FPhraseDirectionalInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhraseDirectionalInput>(NodeName, InChildNodes, InOnInputRecieved)
	{
	}

	FPhraseDirectionalInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhraseDirectionalInput>(NodeName, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
	{
	}
};

class ACCESSIONCOMMUNICATION_API FPhrase2DDirectionalInputNode : public FPhraseEnumInputNode<EPhrase2DDirectionalInput>
{
public:
	FPhrase2DDirectionalInputNode(const TCHAR *NodeName)
		: FPhraseEnumInputNode<EPhrase2DDirectionalInput>(NodeName)
	{
	}

	FPhrase2DDirectionalInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhrase2DDirectionalInput>(NodeName, InChildNodes)
	{
	}

	FPhrase2DDirectionalInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhrase2DDirectionalInput>(NodeName, InOnPhraseParsed, InChildNodes)
	{
	}

	FPhrase2DDirectionalInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhrase2DDirectionalInput>(NodeName, InChildNodes, InOnInputRecieved)
	{
	}

	FPhrase2DDirectionalInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhrase2DDirectionalInput>(NodeName, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
	{
	}
};

class ACCESSIONCOMMUNICATION_API FPhraseScrollInputNode : public FPhraseEnumInputNode<EPhraseScrollInput>
{
public:
	FPhraseScrollInputNode(const TCHAR *NodeName)
		: FPhraseEnumInputNode<EPhraseScrollInput>(NodeName)
	{
	}

	FPhraseScrollInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhraseScrollInput>(NodeName, InChildNodes)
	{
	}

	FPhraseScrollInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhraseScrollInput>(NodeName, InOnPhraseParsed, InChildNodes)
	{
	}

	FPhraseScrollInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhraseScrollInput>(NodeName, InChildNodes, InOnInputRecieved)
	{
	}

	FPhraseScrollInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhraseScrollInput>(NodeName, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
	{
	}
};

class ACCESSIONCOMMUNICATION_API FPhrasePositionalInputNode : public FPhraseEnumInputNode<EPhrasePositionalInput>
{
public:
	FPhrasePositionalInputNode(const TCHAR *NodeName)
		: FPhraseEnumInputNode<EPhrasePositionalInput>(NodeName)
	{
	}

	FPhrasePositionalInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhrasePositionalInput>(NodeName, InChildNodes)
	{
	}

	FPhrasePositionalInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes)
		: FPhraseEnumInputNode<EPhrasePositionalInput>(NodeName, InOnPhraseParsed, InChildNodes)
	{
	}

	FPhrasePositionalInputNode(const TCHAR *NodeName, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhrasePositionalInput>(NodeName, InChildNodes, InOnInputRecieved)
	{
	}

	FPhrasePositionalInputNode(const TCHAR *NodeName, TDelegate<void(FParseRecord &Record)> InOnPhraseParsed, TPhraseNodeArray InChildNodes, TDelegate<void(int32 Input)> InOnInputRecieved)
		: FPhraseEnumInputNode<EPhrasePositionalInput>(NodeName, InOnPhraseParsed, InChildNodes, InOnInputRecieved)
	{
	}
};