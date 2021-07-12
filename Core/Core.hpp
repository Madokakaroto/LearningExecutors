#pragma once

#include "Core/Forward.hpp"
#include "Core/Traits.hpp"
#include "Core/Concepts/Std.hpp"
#include "Core/Concepts/Basic.hpp"
#include "Core/Concepts/Properties.hpp"
#include "Core/CPOs/Properties.hpp"
#include "Core/CPOs/SetValue.hpp"
#include "Core/CPOs/SetError.hpp"
#include "Core/CPOs/SetDone.hpp"
#include "Core/CPOs/Start.hpp"
#include "Core/Concepts/Receiver.hpp"
#include "Core/Concepts/Sender.hpp"
#include "Core/Concepts/OperationState.hpp"
// sender to concepts is defined after the definition of execution::connect cpo
#include "Core/CPOs/Connect.hpp"
#include "Core/Concepts/SenderTo.hpp"

#include "Core/CPOs/Submit.hpp"
// executor of impl and executor concepts are defined after the definition of execution::execute cpo
#include "Core/CPOs/Execute.hpp"
#include "Core/Concepts/Executor.hpp"
#include "Core/Properties.hpp"
// impl of forward traits
#include "Core/TraitsImpl.hpp"

#include "Core/CPOs/BulkExecute.hpp"
// scheduler concept is defined after the definition of execution::shcdule cpo
#include "Core/CPOs/Schedule.hpp"
#include "Core/Concepts/Scheduler.hpp"

// adaptors
#include "Core/Adaptors/AsErrorReceiver.hpp"
#include "Core/Adaptors/AsDoneReceiver.hpp"
#include "Core/Adaptors/AsValueReceiver.hpp"

// standard extensions for the above CPOs
#include "Core/CPOs/StandardExt.hpp"