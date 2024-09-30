.section .vectors, "ax"
.code 32

.globl ExceptionVectors
ExceptionVectors:
  ldr pc, _ResetException
  ldr pc, _UndefInstrException
  ldr pc, _SwiException
  ldr pc, _PrefetchAbortException
  ldr pc, _DataAbortAbortException
  ldr pc, _ResvException
  ldr pc, _IrqException
  ldr pc, _FiqException

.globl Reset_Handler
.globl UndefInstrExceptionHandle
.globl SwiExceptionHandle
.globl PrefetchAbortExceptionHandle
.globl DataAbortExceptionHandle
.globl ResvExceptionHandle
.globl ExceptionIsrEntry
.globl FiqExceptionHandle

_ResetException:
  .word Reset_Handler
_UndefInstrException:
  .word UndefInstrExceptionHandle
_SwiException:
  .word SwiExceptionHandle
_PrefetchAbortException:
  .word PrefetchAbortExceptionHandle
_DataAbortAbortException:
  .word DataAbortExceptionHandle
_ResvException:
  .word ResvExceptionHandle
_IrqException:
  .word ExceptionIsrEntry
_FiqException:
  .word FiqExceptionHandle

.globl _start
Reset_Handler:
    b     _start

UndefInstrExceptionHandle:
    b     UndefInstrIsrEntry

SwiExceptionHandle:
    b     SvcIsrEntry

PrefetchAbortExceptionHandle:
    b     PrefetchAbortIsrEntry

DataAbortExceptionHandle:
    b     UndefInstrIsrEntry

ResvExceptionHandle:
    b     DataAbortIsrEntry

ExceptionIsrEntry:
    stmfd   sp!, {r0-r12,lr}

    bl      IsrEntry

FiqExceptionHandle:
    b      FiqIsrEntry
