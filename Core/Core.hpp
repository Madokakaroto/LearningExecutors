#pragma once

#include "Core/Forward.hpp"
#include "Core/Traits.hpp"

// basic
#include "Core/Concepts/Std.hpp"
#include "Core/Concepts/Basic.hpp"

// fundamental CPOs
#include "Core/CPOs/Properties.hpp"
#include "Core/CPOs/SetValue.hpp"
#include "Core/CPOs/SetError.hpp"
#include "Core/CPOs/SetDone.hpp"

// CPOs concepts
#include "Core/Concepts/Receiver.hpp"
#include "Core/Concepts/Sender.hpp"
#include "Core/Concepts/OperationState.hpp"
#include "Core/Concepts/Scheduler.hpp"
#include "Core/Concepts/Executor.hpp"

// properties
#include "Core/Properties.hpp"

// core CPOs
#include "Core/CPOs/Start.hpp"
#include "Core/CPOs/Connect.hpp"
#include "Core/CPOs/Submit.hpp"
#include "Core/CPOs/Execute.hpp"
#include "Core/CPOs/BulkExecute.hpp"
#include "Core/CPOs/Schedule.hpp"

// traits impl
#include "Core/TraitsImpl.hpp"

// adaptors
#include "Core/Adaptors/AsErrorReceiver.hpp"
#include "Core/Adaptors/AsDoneReceiver.hpp"
#include "Core/Adaptors/AsValueReceiver.hpp"

// standard extensions for the above CPOs
#include "Core/CPOs/StandardExt.hpp"