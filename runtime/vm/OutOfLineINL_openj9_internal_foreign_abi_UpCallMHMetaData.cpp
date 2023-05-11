/*******************************************************************************
 * Copyright IBM Corp. and others 2021
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] https://openjdk.org/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "j9vmnls.h"
#include "AtomicSupport.hpp"
#include "BytecodeAction.hpp"
#include "LayoutFFITypeHelpers.hpp"
#include "OutOfLineINL.hpp"
#include "VMHelpers.hpp"
#include "UnsafeAPI.hpp"

extern "C" {

#if JAVA_SPEC_VERSION >= 16
/* openj9.internal.foreign.abi.UpcallMHMetaData: private static synchronized native void resolveUpcallDataInfo(); */
VM_BytecodeAction
OutOfLineINL_openj9_internal_foreign_abi_UpcallMHMetaData_resolveUpcallDataInfo(J9VMThread *currentThread, J9Method *method)
{
	VM_BytecodeAction rc = EXECUTE_BYTECODE;
	J9JavaVM *vm = currentThread->javaVM;
	J9ConstantPool *jclConstantPool = (J9ConstantPool *)vm->jclConstantPool;
	J9ROMClass * jclROMClass = jclConstantPool->ramClass->romClass;
	U_32 * cpShapeDescription = J9ROMCLASS_CPSHAPEDESCRIPTION(jclROMClass);
#if JAVA_SPEC_VERSION >= 20
	const int cpEntryNum = 10;
#elif JAVA_SPEC_VERSION >= 18 /* JAVA_SPEC_VERSION >= 20 */
	const int cpEntryNum = 11;
#else /* JAVA_SPEC_VERSION >= 18 */
	const int cpEntryNum = 12;
#endif /* JAVA_SPEC_VERSION >= 20 */
	U_16 cpIndex[cpEntryNum] = {
#if JAVA_SPEC_VERSION >= 20
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_GETNATIVEARGRETSEGMENTOFPTR,
#else /* JAVA_SPEC_VERSION >= 20 */
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_GETNATIVEARGRETADDROFPTR,
#endif /* JAVA_SPEC_VERSION >= 20 */
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_GETNATIVEARGRETSEGMENT,
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_CALLEEMH,
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_CALLEETYPE,
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_INVOKECACHE,
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_NATIVEARGARRAY,
#if JAVA_SPEC_VERSION == 19
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_SESSION,
#else /* JAVA_SPEC_VERSION == 19 */
			J9VMCONSTANTPOOL_OPENJ9INTERNALFOREIGNABIUPCALLMHMETADATA_SCOPE,
#endif /* JAVA_SPEC_VERSION == 19 */
#if JAVA_SPEC_VERSION <= 19
			J9VMCONSTANTPOOL_JDKINTERNALFOREIGNMEMORYADDRESSIMPL_OFFSET, /* MemoryAddress related classes were removed in JDK20. */
#if JAVA_SPEC_VERSION <= 17
			J9VMCONSTANTPOOL_JDKINTERNALFOREIGNMEMORYADDRESSIMPL_SEGMENT, /* Removed from MemoryAddressImpl since JDK18. */
#endif /* JAVA_SPEC_VERSION <= 17 */
#endif /* JAVA_SPEC_VERSION <= 19 */
			J9VMCONSTANTPOOL_JDKINTERNALFOREIGNNATIVEMEMORYSEGMENTIMPL_MIN,
			J9VMCONSTANTPOOL_JDKINTERNALFOREIGNNATIVEMEMORYSEGMENTIMPL_LENGTH,
#if JAVA_SPEC_VERSION == 19
			J9VMCONSTANTPOOL_JDKINTERNALFOREIGNNATIVEMEMORYSEGMENTIMPL_SESSION
#else /* JAVA_SPEC_VERSION == 19 */
			J9VMCONSTANTPOOL_JDKINTERNALFOREIGNNATIVEMEMORYSEGMENTIMPL_SCOPE
#endif /* JAVA_SPEC_VERSION == 19 */
			};

	VM_OutOfLineINL_Helpers::buildInternalNativeStackFrame(currentThread, method);
	for (int i = 0; i < cpEntryNum; i++) {
		UDATA offset = cpIndex[i];
		UDATA cpType = J9_CP_TYPE(cpShapeDescription, offset);
		UDATA resolveFlags = J9_RESOLVE_FLAG_NO_THROW_ON_FAIL | J9_RESOLVE_FLAG_JCL_CONSTANT_POOL;

		if (J9CPTYPE_STATIC_METHOD == cpType) {
			resolveStaticMethodRef(currentThread, jclConstantPool, offset, resolveFlags);
		} else if (J9CPTYPE_FIELD == cpType) {
			J9RAMFieldRef *cpFieldRef = ((J9RAMFieldRef*)jclConstantPool) + offset;
			UDATA const flags = cpFieldRef->flags;
			UDATA const valueOffset = cpFieldRef->valueOffset;

			if (!VM_VMHelpers::instanceFieldRefIsResolved(flags, valueOffset)) {
				resolveInstanceFieldRef(currentThread, NULL, jclConstantPool, offset, resolveFlags, NULL);
			}
		}

		if (VM_VMHelpers::exceptionPending(currentThread)) {
			rc = GOTO_THROW_CURRENT_EXCEPTION;
			goto done;
		}
	}

	VM_OutOfLineINL_Helpers::restoreInternalNativeStackFrame(currentThread);

done:
	VM_OutOfLineINL_Helpers::returnVoid(currentThread, 0);
	return rc;
}
#endif /* JAVA_SPEC_VERSION >= 16 */

} /* extern "C" */
