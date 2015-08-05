/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-2015 Facebook, Inc. (http://www.facebook.com)     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef incl_HPHP_JIT_VASM_INTERNAL_H_
#define incl_HPHP_JIT_VASM_INTERNAL_H_

#include "hphp/runtime/vm/jit/containers.h"
#include "hphp/runtime/vm/jit/vasm.h"
#include "hphp/runtime/vm/jit/vasm-reg.h"

#include "hphp/util/data-block.h"

namespace HPHP { namespace jit {
///////////////////////////////////////////////////////////////////////////////

struct AsmInfo;
struct Vtext;
struct Vunit;

///////////////////////////////////////////////////////////////////////////////

/*
 * State maintained by vasm_emit().
 */
struct Venv {
  /*
   * Patch data collected at emit-time for post-processing.
   */
  struct LabelPatch { CodeAddress instr; Vlabel target; };

  Venv(const Vunit& unit, Vtext& text) : unit(unit), text(text) {}

  const Vunit& unit;
  Vtext& text;

  CodeBlock* cb;

  Vlabel current{0};
  Vlabel next{0};

  jit::vector<CodeAddress> addrs, points;
  jit::vector<LabelPatch> jmps, jccs, bccs;
  jit::vector<LabelPatch> catches;
};

/*
 * Toplevel vasm assembly-emitter loop.
 *
 * This is an emit loop implementation which provides the two-phase emitter
 * pattern of (1) emitting all code to the TC; then (2) performing patches that
 * we track at emit-time.
 *
 * vasm_emit() creates and updates a Venv object, and delegates the work of
 * emitting single Vinstrs to the Vemit template class.  Vemit is expected to
 * update the various patch point structures on the Venv as needed.
 *
 * Vemit is expected to have the following interface:
 *
 * struct Vemit {
 *   Vemit(Venv&);
 *
 *   // One emit routine for each Vinstr type.
 *   template<class Inst> void emit(Inst& i);
 *
 *   // Add arch-specific trap instruction padding up to the end of the block.
 *   static void pad(CodeBlock&);
 *
 *   // Perform all the accumulated patches.
 *   static void patch(Venv&);
 * };
 */
template<class Vemit>
void vasm_emit(const Vunit& u, Vtext& text, AsmInfo* asm_info);

///////////////////////////////////////////////////////////////////////////////
}}

#include "hphp/runtime/vm/jit/vasm-internal-inl.h"

#endif