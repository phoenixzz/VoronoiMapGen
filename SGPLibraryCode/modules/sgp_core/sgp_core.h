#ifndef __SGP_CORE_HEADER__
#define __SGP_CORE_HEADER__


//==============================================================================
#include "system/sgp_TargetPlatform.h"



//=============================================================================
//=============================================================================
#if SGP_MSVC
 #pragma warning (disable: 4251) // (DLL build warning, must be disabled before pushing the warning state)
 #pragma warning (push)
 #pragma warning (disable: 4786) // (long class name warning)
 #ifdef __INTEL_COMPILER
  #pragma warning (disable: 1125)
 #endif
#endif

#include "system/sgp_StandardHeader.h"

namespace sgp
{
#ifndef __SPG_DATATYPE_HEADER__
 #include "common/sgp_DataType.h"
#endif
/*

#ifndef __JUCE_ABSTRACTFIFO_JUCEHEADER__
 #include "containers/juce_AbstractFifo.h"
#endif
 */

#ifndef __SGP_ARRAY_HEADER__
 #include "common/sgp_Array.h"
#endif
#ifndef __SGP_ARRAYALLOCATIONBASE_HEADER__
 #include "common/sgp_ArrayAllocationBase.h"
#endif
#ifndef __SGP_ARRAYSTACK_HEADER__
 #include "common/sgp_ArrayStack.h"
#endif
/*
#ifndef __JUCE_DYNAMICOBJECT_JUCEHEADER__
 #include "containers/juce_DynamicObject.h"
#endif
*/

#ifndef __SGP_ELEMENTCOMPARATOR_HEADER__
 #include "common/sgp_ElementComparator.h"
#endif
#ifndef __SGP_HASHMAP_HEADER__
 #include "common/sgp_HashMap.h"
#endif
#ifndef __SGP_LINKEDLISTPOINTER_HEADER__
 #include "common/sgp_LinkedListPointer.h"
#endif
#ifndef __SGP_RINGFIFO_HEADER__
 #include "common/sgp_RingFIFO.h"
#endif
/*
#ifndef __JUCE_NAMEDVALUESET_JUCEHEADER__
 #include "containers/juce_NamedValueSet.h"
#endif
 */
#ifndef __SGP_OWNEDARRAY_HEADER__
 #include "common/sgp_OwnedArray.h"
#endif
/*
#ifndef __JUCE_PROPERTYSET_JUCEHEADER__
 #include "containers/juce_PropertySet.h"
#endif
#ifndef __JUCE_REFERENCECOUNTEDARRAY_JUCEHEADER__
 #include "containers/juce_ReferenceCountedArray.h"
#endif
#ifndef __JUCE_SCOPEDVALUESETTER_JUCEHEADER__
 #include "containers/juce_ScopedValueSetter.h"
#endif
#ifndef __JUCE_SORTEDSET_JUCEHEADER__
 #include "containers/juce_SortedSet.h"
#endif
#ifndef __JUCE_SPARSESET_JUCEHEADER__
 #include "containers/juce_SparseSet.h"
#endif
#ifndef __JUCE_VARIANT_JUCEHEADER__
 #include "containers/juce_Variant.h"
#endif
 */
#ifndef __SGP_DIRECTORYITERATOR_HEADER__
 #include "files/sgp_DirectoryIterator.h"
#endif
#ifndef __SGP_FILE_HEADER__
 #include "files/sgp_File.h"
#endif
#ifndef __SGP_FILEINPUTSTREAM_HEADER__
 #include "files/sgp_FileInputStream.h"
#endif
#ifndef __SGP_FILEOUTPUTSTREAM_HEADER__
 #include "files/sgp_FileOutputStream.h"
#endif
/*
#ifndef __JUCE_FILESEARCHPATH_JUCEHEADER__
 #include "files/juce_FileSearchPath.h"
#endif
#ifndef __JUCE_MEMORYMAPPEDFILE_JUCEHEADER__
 #include "files/juce_MemoryMappedFile.h"
#endif
#ifndef __JUCE_TEMPORARYFILE_JUCEHEADER__
 #include "files/juce_TemporaryFile.h"
#endif
 */

#ifndef __SGP_LOGGER_HEADER__
 #include "log/sgp_Logger.h"
#endif
#ifndef __SGP_FILELOGGER_HEADER__
 #include "log/sgp_FileLogger.h"
#endif
#ifndef __SGP_CONSOLELOGGER_HEADER__
 #include "log/sgp_ConsoleLogger.h"
#endif

/*
#ifndef __JUCE_BIGINTEGER_JUCEHEADER__
 #include "maths/juce_BigInteger.h"
#endif
#ifndef __JUCE_EXPRESSION_JUCEHEADER__
 #include "maths/juce_Expression.h"
#endif

#ifndef __JUCE_RANDOM_JUCEHEADER__
 #include "maths/juce_Random.h"
#endif
#ifndef __JUCE_RANGE_JUCEHEADER__
 #include "maths/juce_Range.h"
#endif
 */
#ifndef __SGP_ATOMIC_HEADER__
 #include "common/sgp_Atomic.h"
#endif

#ifndef __SGP_BYTEORDER_HEADER__
 #include "common/sgp_ByteOrder.h"
#endif
#ifndef __SGP_HEAPBLOCK_HEADER__
 #include "common/sgp_HeapBlock.h"
#endif
/*
#ifndef __JUCE_LEAKEDOBJECTDETECTOR_JUCEHEADER__
 #include "memory/juce_LeakedObjectDetector.h"
#endif
 */
#ifndef __SGP_MEMORY_HEADER__
 #include "common/sgp_Memory.h"
#endif

#ifndef __SGP_MEMORYBLOCK_HEADER__
 #include "common/sgp_MemoryBlock.h"
#endif
/*
#ifndef __JUCE_OPTIONALSCOPEDPOINTER_JUCEHEADER__
 #include "memory/juce_OptionalScopedPointer.h"
#endif 
 */
#ifndef __SGP_REFERENCECOUNTEDOBJECT_HEADER__
 #include "common/sgp_ReferenceCountedObject.h"
#endif

#ifndef __SGP_SCOPEDPOINTER_HEADER__
 #include "common/sgp_ScopedPointer.h"
#endif
#ifndef __SGP_SINGLETON_HEADER__
 #include "common/sgp_Singleton.h"
#endif
/*
#ifndef __JUCE_WEAKREFERENCE_JUCEHEADER__
 #include "memory/juce_WeakReference.h"
#endif
 */
#ifndef __SGP_RESULT_HEADER__
 #include "common/sgp_Result.h"
#endif
#ifndef __SGP_COLOUR_HEADER__
 #include "common/sgp_Colour.h"
#endif
#ifndef __SGP_SORTEDSET_HEADER__
 #include "common/sgp_SortedSet.h"
#endif
#ifndef __SGP_RANDOM_HEADER__
 #include "common/sgp_Random.h"
#endif
/*
#ifndef __JUCE_WINDOWSREGISTRY_JUCEHEADER__
 #include "misc/juce_WindowsRegistry.h"
#endif

#ifndef __JUCE_BUFFEREDINPUTSTREAM_JUCEHEADER__
 #include "streams/juce_BufferedInputStream.h"
#endif

 */

 
#ifndef __SGP_INPUTSTREAM_HEADER__
 #include "streams/sgp_InputStream.h"
#endif
#ifndef __SGP_MEMORYINPUTSTREAM_HEADER__
 #include "streams/sgp_MemoryInputStream.h"
#endif
#ifndef __SGP_MEMORYOUTPUTSTREAM_HEADER__
 #include "streams/sgp_MemoryOutputStream.h"
#endif 
#ifndef __SGP_OUTPUTSTREAM_HEADER__
 #include "streams/sgp_OutputStream.h"
#endif
#ifndef __SGP_INPUTSOURCE_HEADER__
 #include "streams/sgp_InputSource.h"
#endif
#ifndef __SGP_FILEINPUTSOURCE_HEADER__
 #include "streams/sgp_FileInputSource.h"
#endif
/*
#ifndef __JUCE_SUBREGIONSTREAM_JUCEHEADER__
 #include "streams/juce_SubregionStream.h"
#endif
 */
#ifndef __SGP_PLATFORMDEFS_HEADER__
 #include "system/sgp_PlatformDefs.h"
#endif
#ifndef __SGP_STANDARDHEADER_HEADER__
 #include "system/sgp_StandardHeader.h"
#endif
#ifndef __SGP_SYSTEMSTATS_HEADER__
 #include "system/sgp_SystemStats.h"
#endif
#ifndef __SGP_TARGETPLATFORM_HEADER__
 #include "system/sgp_TargetPlatform.h"
#endif

#ifndef __SGP_CHARACTERFUNCTIONS_HEADER__
 #include "text/sgp_CharacterFunctions.h"
#endif
#ifndef __SGP_CHARPOINTER_ASCII_HEADER__
 #include "text/sgp_CharPointer_ASCII.h"
#endif
#ifndef __SGP_CHARPOINTER_UTF16_HEADER__
 #include "text/sgp_CharPointer_UTF16.h"
#endif
#ifndef __SGP_CHARPOINTER_UTF32_HEADER__
 #include "text/sgp_CharPointer_UTF32.h"
#endif
#ifndef __SGP_CHARPOINTER_UTF8_HEADER__
 #include "text/sgp_CharPointer_UTF8.h"
#endif
/*
#ifndef __JUCE_IDENTIFIER_JUCEHEADER__
 #include "text/juce_Identifier.h"
#endif
#ifndef __JUCE_LOCALISEDSTRINGS_JUCEHEADER__
 #include "text/juce_LocalisedStrings.h"
#endif
 */
#ifndef __SGP_NEWLINE_HEADER__
 #include "text/sgp_NewLine.h"
#endif

#ifndef __SGP_STRING_HEADER__
 #include "text/sgp_String.h"
#endif
#ifndef __SGP_STRINGARRAY_HEADER__
 #include "text/sgp_StringArray.h"
#endif
/*
#ifndef __JUCE_STRINGPAIRARRAY_JUCEHEADER__
 #include "text/juce_StringPairArray.h"
#endif
*/
#ifndef __SGP_STRINGPOOL_HEADER__
 #include "text/sgp_StringPool.h"
#endif
/*
#ifndef __JUCE_TEXTDIFF_JUCEHEADER__
 #include "text/juce_TextDiff.h"
#endif
#ifndef __JUCE_CHILDPROCESS_JUCEHEADER__
 #include "threads/juce_ChildProcess.h"
#endif
 */
#ifndef __SGP_CRITICALSECTION_HEADER__
 #include "threads/sgp_CriticalSection.h"
#endif

#ifndef __SGP_DYNAMICLIBRARY_HEADER__
 #include "threads/sgp_DynamicLibrary.h"
#endif
/*
#ifndef __JUCE_INTERPROCESSLOCK_JUCEHEADER__
 #include "threads/juce_InterProcessLock.h"
#endif
 */
#ifndef __SGP_PROCESS_HEADER__
 #include "threads/sgp_Process.h"
#endif
/*
#ifndef __JUCE_READWRITELOCK_JUCEHEADER__
 #include "threads/juce_ReadWriteLock.h"
#endif
 */
#ifndef __SGP_SCOPEDLOCK_HEADER__
 #include "threads/sgp_ScopedLock.h"
#endif
/*
#ifndef __JUCE_SCOPEDREADLOCK_JUCEHEADER__
 #include "threads/juce_ScopedReadLock.h"
#endif
#ifndef __JUCE_SCOPEDWRITELOCK_JUCEHEADER__
 #include "threads/juce_ScopedWriteLock.h"
#endif
 */
#ifndef __SGP_SPINLOCK_HEADER__
 #include "threads/sgp_SpinLock.h"
#endif

#ifndef __SGP_THREAD_HEADER__
 #include "threads/sgp_Thread.h"
#endif

#ifndef __SGP_THREADLOCALVALUE_HEADER__
 #include "threads/sgp_ThreadLocalValue.h"
#endif
/*
#ifndef __JUCE_THREADPOOL_JUCEHEADER__
 #include "threads/juce_ThreadPool.h"
#endif
#ifndef __JUCE_TIMESLICETHREAD_JUCEHEADER__
 #include "threads/juce_TimeSliceThread.h"
#endif
 */
#ifndef __SGP_WAITABLEEVENT_HEADER__
 #include "threads/sgp_WaitableEvent.h"
#endif
/*
#ifndef __JUCE_PERFORMANCECOUNTER_JUCEHEADER__
 #include "time/sgp_PerformanceCounter.h"
#endif
 */
#ifndef __SGP_RELATIVETIME_HEADER__
 #include "time/sgp_RelativeTime.h"
#endif
#ifndef __SGP_TIME_HEADER__
 #include "time/sgp_Time.h"
#endif

#ifndef __SGP_XMLELEMENT_HEADER__
 #include "xml/sgp_XmlElement.h"
#endif
#ifndef __SGP_XMLDOCUMENT_HEADER__
 #include "xml/sgp_XmlDocument.h"
#endif
/*
#ifndef __JUCE_UNITTEST_JUCEHEADER__
 #include "unit_tests/juce_UnitTest.h"
#endif
#ifndef __JUCE_GZIPCOMPRESSOROUTPUTSTREAM_JUCEHEADER__
 #include "zip/juce_GZIPCompressorOutputStream.h"
#endif
#ifndef __JUCE_GZIPDECOMPRESSORINPUTSTREAM_JUCEHEADER__
 #include "zip/juce_GZIPDecompressorInputStream.h"
#endif
#ifndef __JUCE_ZIPFILE_JUCEHEADER__
 #include "zip/juce_ZipFile.h"
#endif
// END_AUTOINCLUDE
*/

}

#if SGP_MSVC
 #pragma warning (pop)
#endif

#endif   // __SGP_CORE_HEADER__
