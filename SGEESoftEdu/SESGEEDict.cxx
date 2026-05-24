// =============================================================================
// SGEE SoftEdu - Stern-Gerlach Experiment Experience (Installer Script)
// =============================================================================
//
// Copyright (C) 2026
//
// Authors:
//   [1] Julián Salamanca*
//   [2] Diego Julián Rodríguez-Patarroyo**
//
//   [1] jasalamanca@udistrital.edu.co (profesor Universidad Distrital)
//   [2] djrodriguezp@udistrital.edu.co (profesor Universidad Distrital)
//
//  * Grupo de Física e Informática (FISINFOR)
//  ** Grupo de Laboratorio de Fuentes Alternas de Energía (LIFAE)
//  *,** Universidad Distrital Francisco José de Caldas (Bogotá, Colombia)
//  
// Web page: https://github.com/fisinforgh/edpm
//
// LICENSE: GNU General Public License v3.0 (GPLv3) or later
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// =============================================================================
// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME SESGEEDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "SESGEE.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void delete_SGIntegratedGUI(void *p);
   static void deleteArray_SGIntegratedGUI(void *p);
   static void destruct_SGIntegratedGUI(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SGIntegratedGUI*)
   {
      ::SGIntegratedGUI *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::SGIntegratedGUI >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("SGIntegratedGUI", ::SGIntegratedGUI::Class_Version(), "SESGEE.h", 22,
                  typeid(::SGIntegratedGUI), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::SGIntegratedGUI::Dictionary, isa_proxy, 4,
                  sizeof(::SGIntegratedGUI) );
      instance.SetDelete(&delete_SGIntegratedGUI);
      instance.SetDeleteArray(&deleteArray_SGIntegratedGUI);
      instance.SetDestructor(&destruct_SGIntegratedGUI);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SGIntegratedGUI*)
   {
      return GenerateInitInstanceLocal(static_cast<::SGIntegratedGUI*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::SGIntegratedGUI*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr SGIntegratedGUI::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *SGIntegratedGUI::Class_Name()
{
   return "SGIntegratedGUI";
}

//______________________________________________________________________________
const char *SGIntegratedGUI::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SGIntegratedGUI*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int SGIntegratedGUI::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::SGIntegratedGUI*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *SGIntegratedGUI::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SGIntegratedGUI*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *SGIntegratedGUI::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::SGIntegratedGUI*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void SGIntegratedGUI::Streamer(TBuffer &R__b)
{
   // Stream an object of class SGIntegratedGUI.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(SGIntegratedGUI::Class(),this);
   } else {
      R__b.WriteClassBuffer(SGIntegratedGUI::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_SGIntegratedGUI(void *p) {
      delete (static_cast<::SGIntegratedGUI*>(p));
   }
   static void deleteArray_SGIntegratedGUI(void *p) {
      delete [] (static_cast<::SGIntegratedGUI*>(p));
   }
   static void destruct_SGIntegratedGUI(void *p) {
      typedef ::SGIntegratedGUI current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::SGIntegratedGUI

namespace ROOT {
   // Registration Schema evolution read functions
   int RecordReadRules_SESGEEDict() {
      return 0;
   }
   static int _R__UNIQUE_DICT_(ReadRules_SESGEEDict) = RecordReadRules_SESGEEDict();R__UseDummy(_R__UNIQUE_DICT_(ReadRules_SESGEEDict));
} // namespace ROOT
namespace {
  void TriggerDictionaryInitialization_SESGEEDict_Impl() {
    static const char* headers[] = {
"SESGEE.h",
nullptr
    };
    static const char* includePaths[] = {
"/home/jusalama/software/root/include/",
"/home/jusalama/software/SGEESoftEdu/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "SESGEEDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$SESGEE.h")))  SGIntegratedGUI;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "SESGEEDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "SESGEE.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"SGIntegratedGUI", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("SESGEEDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_SESGEEDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_SESGEEDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_SESGEEDict() {
  TriggerDictionaryInitialization_SESGEEDict_Impl();
}
