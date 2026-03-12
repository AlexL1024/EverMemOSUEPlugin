# EverMemOS UE Plugin

Unreal Engine plugin for [EverMemOS](https://evermind.ai) — Memory-as-a-Service for AI-powered games. Give your NPCs persistent memory so they remember what players said, did, and experienced.

Supports both **Cloud** (api.evermind.ai) and **Local** (localhost) deployment.

## Requirements

- Unreal Engine 5.3+
- For Local mode: a running EverMemOS local server on `localhost:1995`

## Installation

### As a Git Submodule (Recommended)

```bash
cd YourProject/Plugins
git submodule add https://github.com/AlexL1024/EverMemOSUEPlugin.git EverMemOS
```

### Manual Copy

Clone or download this repo into your project's `Plugins/EverMemOS/` directory.

### Enable the Plugin

In your `.uproject` file, add:

```json
{
  "Plugins": [
    {
      "Name": "EverMemOS",
      "Enabled": true
    }
  ]
}
```

Rebuild your project.

## Configuration

Open **Edit > Project Settings > Plugins > EverMemOS**.

| Setting | Description |
|---------|-------------|
| **Deployment Profile** | `Cloud` or `Local` — auto-sets BaseURL and API version |
| **Base URL** | API endpoint (auto-filled by profile) |
| **Api Version** | `v0` for Cloud, `v1` for Local |
| **Auth Method** | `Bearer Token` or `HMAC-SHA256` (Cloud only) |
| **Timeout Seconds** | Request timeout (default: 30s) |
| **Max Retries** | Auto-retry count for 429/5xx errors (default: 3) |

### Cloud Setup

1. Set **Deployment Profile** to `Cloud`
2. Enter your Bearer Token or HMAC Secret in the Authentication section
3. Tokens are transient (not saved to config files) — set them at runtime in Blueprints or C++

### Local Setup

1. Start your EverMemOS local server (default: `http://localhost:1995`)
2. Set **Deployment Profile** to `Local`
3. No authentication needed

## Blueprint Usage

All memory operations are exposed as **async Blueprint nodes** with `OnSuccess` and `OnFailure` output pins.

### Switch Deployment Profile at Runtime

```
Set Deployment Profile (Profile = Local)
```

### Memorize (Upload Conversation)

Submit a conversation for the server to extract and store memories:

```
Make Message (Role = User, Content = "I need help finding the dragon")
  -> Make Array
    -> Make Memorize Request (UserId = "player_01", Messages = Array)
      -> Memorize
          OnSuccess -> Status, Message
          OnFailure -> Error
```

### Add Memory (Upload Single Message)

Add a single raw message to the memory store:

```
Utc Now Iso8601 -> Timestamp
Make Add Memory Request
  MessageId = "msg_001"
  Content   = "The player gave the guard a sword"
  Sender    = "npc_guard"
  CreateTime = Timestamp
  GroupId   = "quest_01"
-> Add Memory
    OnSuccess -> RequestId
    OnFailure -> Error
```

> **Note:** In AddMemory, `Sender` serves as the user identifier. When searching/getting memories later, use `UserId = "npc_guard"` to retrieve this NPC's memories.

### Search Memories

```
Make Search Params
  UserId = "npc_guard"
  Query  = "What gift did the player give?"
  Method = Hybrid
  TopK   = 5
-> Search Memories
    OnSuccess -> Result.Memories (array), Result.TotalCount
    OnFailure -> Error
```

### Get Memories (Paginated)

```
Make Get Memories Params
  UserId  = "npc_guard"
  GroupIds = ["quest_01"]
  Page     = 1
  PageSize = 20
-> Get Memories
    OnSuccess -> Result.Memories, Result.Count, Result.TotalCount
    OnFailure -> Error
```

### Delete Memories

```
Make Delete Memories Request
  UserId  = "npc_guard"
  GroupId = "quest_01"
-> Delete Memories
    OnSuccess -> Count
    OnFailure -> Error
```

### Health Check

```
Health Check
  OnSuccess -> Message, Service
  OnFailure -> Error (server not reachable)
```

### Conversation Metadata

```
-- Get --
Get Conversation Metadata (GroupId = "quest_01")
  OnSuccess -> Id, Name, CreatedAt, UpdatedAt

-- Set --
Make Convo Meta Request (GroupId = "quest_01", Name = "Dragon Quest Chat", CreatedAt = Timestamp)
-> Set Conversation Metadata
    OnSuccess -> Updated metadata
```

## C++ Usage

Access the subsystem from any `UObject` with a world context:

```cpp
#include "EverMemOS/EverMemOSSubsystem.h"

UGameInstance* GI = GetWorld()->GetGameInstance();
UEverMemOSSubsystem* MemOS = GI->GetSubsystem<UEverMemOSSubsystem>();

// Switch to local mode
MemOS->SetDeploymentProfile(EEverMemOSDeploymentProfile::Local);

// Search memories
FEverMemOSSearchParams Params;
Params.UserId = TEXT("npc_guard");
Params.Query = TEXT("What happened yesterday?");
Params.RetrieveMethod = EEverMemOSRetrieveMethod::Hybrid;
Params.TopK = 5;

MemOS->Search(Params, FOnSearchComplete::CreateLambda(
    [](const FEverMemOSSearchResult& Result, const FEverMemOSError& Error)
    {
        if (Error.IsError())
        {
            UE_LOG(LogTemp, Error, TEXT("Search failed: %s"), *Error.Message);
            return;
        }
        for (const FEverMemOSMemoryRecord& Memory : Result.Memories)
        {
            UE_LOG(LogTemp, Log, TEXT("Memory: %s"), *Memory.Content);
        }
    }));
```

## Key Concepts

| Concept | Description |
|---------|-------------|
| **UserId** | Identifies whose memory it is (e.g., `"npc_guard"`, `"player_01"`) |
| **GroupId** | Groups memories by conversation or scenario (e.g., `"quest_01"`) |
| **SessionId** | Links messages within a single session (used in Memorize) |
| **Sender** | In AddMemory, this is the user identifier (equivalent to UserId) |
| **MemoryType** | `Profile`, `EpisodicMemory`, `Foresight`, `EventLog`, `GroupProfile` |
| **RetrieveMethod** | `Keyword`, `Vector`, `Hybrid`, `RRF`, `Agentic` |

## API Reference

### Async Blueprint Nodes

| Node | Description |
|------|-------------|
| `Memorize` | Submit conversation messages for memory extraction |
| `Add Memory` | Add a single raw message |
| `Search Memories` | Search memories by query |
| `Get Memories` | Retrieve memories with pagination |
| `Delete Memories` | Delete memories by filters |
| `Health Check` | Check server connectivity |
| `Get Conversation Metadata` | Get conversation group info |
| `Set Conversation Metadata` | Create/update conversation group info |

### Blueprint Library Helpers

| Function | Description |
|----------|-------------|
| `Make Message` | Create a message struct |
| `Make Memorize Request` | Build a memorize request |
| `Make Search Params` | Build search parameters |
| `Make Add Memory Request` | Build an add memory request |
| `Make Get Memories Params` | Build get memories parameters |
| `Make Delete Memories Request` | Build a delete request |
| `Utc Now Iso8601` | Get current UTC time as ISO 8601 string |
| `Error To String` | Convert error to readable string |
| `Set Deployment Profile` | Switch Cloud/Local at runtime |
| `Get Deployment Profile` | Get current deployment profile |

## License

See [LICENSE](LICENSE) for details.
