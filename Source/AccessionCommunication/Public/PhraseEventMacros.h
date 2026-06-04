#pragma once


#define UPHRASE_EVENT_META(CommandId, DefaultPhrase, ContextName) \
	AccessionPhraseEvent = "true", \
	AccessionCommandId = CommandId, \
	AccessionDefaultPhrase = DefaultPhrase, \
	AccessionContext = ContextName \

#define UPHRASE_EVENT_META_EX(CommandId, DefaultPhrase, ContextName, ...) \
	UPHRASE_EVENT_META(CommandId, DefaultPhrase, ContextName), \
	__VA_ARGS__ \

#define UPHRASE_EVENT(CommandId, DefaultPhrase, ContextName) \
	UFUNCTION(meta = ( \
		UPHRASE_EVENT_META(CommandId, DefaultPhrase, ContextName) \
	)) \

#define UPHRASE_EVENT_EX(CommandId, DefaultPhrase, ContextName, ...) \
	UFUNCTION(meta = ( \
		UPHRASE_EVENT_META_EX(CommandId, DefaultPhrase, ContextName, __VA_ARGS__) \
	)) \