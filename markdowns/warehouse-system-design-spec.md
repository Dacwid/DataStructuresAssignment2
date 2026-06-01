# Warehouse Robot Navigation System — Design Spec

**Module:** CT077-3-2-DSTR · Lab Evaluation Work #2
**Purpose of this document:** the shared foundation every member builds against — data model, module responsibilities, how the modules connect, and the team conventions.

> This is a **design spec**, not source code. It describes *what* each module must do, what it stores, and the rules to build to. The actual C++ (the data structures, algorithms, and logic) is each member's own work — required by the assignment's AI policy, and necessary because each member defends their own code in the live Q&A.

---

## 1. System overview

Orders arrive, get queued, are assigned to robots in rotation, the item's location is looked up, a route is generated through the warehouse, the robot walks it step by step, and then returns by the exact reverse route. Five modules, one per member, each anchored on one data structure.

| Member | Module | Data structure | Why it fits |
|---|---|---|---|
| 1 | Order Management | Queue (FIFO) | Orders served in arrival order = fair |
| 2 | Robot Assignment | Circular queue | Rotating assignment that wraps without restarting |
| 3 | Navigation & Path Tracking | Stack (LIFO) | Last step out = first step on the reverse return |
| 4 | Item Search | Binary search tree | Fast lookup by ID; in-order traversal = sorted display |
| 5 | Warehouse Layout | N-ary (hierarchical) tree | Zone → aisle → shelf containment; DFS generates routes |

**Data flow:** Order Management → Robot Assignment → Item Search → Warehouse Layout → Navigation. The "currency" passed at each handoff is defined in section 4.

---

## 2. Shared data model (`Types.hpp`)

Defined once, included by every module. These are the contents — write the actual structs/enums yourselves.

**Location** — a fixed array of three integers: `zone`, `aisle`, `shelf`. These are *hierarchy indices*, not free coordinates: an aisle number only has meaning inside its zone, and a shelf inside its aisle. That containment is what makes the warehouse a tree.

**Direction** — an enum of moves Navigation records: `forward`, `backward`, `left`, `right`. Each has an opposite, used when reversing.

**Order**

| Field | Type | Notes |
|---|---|---|
| orderID | integer | unique |
| itemID | integer | one item per order |
| status | enum | pending / assigned / completed |

No arrival timestamp — the queue's FIFO order *is* the arrival order.

**Robot**

| Field | Type | Notes |
|---|---|---|
| robotID | integer | unique |
| status | enum | available / busy / maintenance |
| taskCount | integer | tasks handled (rotation fairness + tracking) |

**Item**

| Field | Type | Notes |
|---|---|---|
| itemID | integer | BST key |
| name | string | |
| location | Location | zone / aisle / shelf |

**Route** — a fixed array of `Location` plus a `count`. This is also the general rule for passing any sequence between modules: **fixed array + count** (no STL containers).

**MAX constants** (placeholders — bump as needed):

| Constant | Value |
|---|---|
| MAX_ORDERS | 100 |
| MAX_ROBOTS | 10 |
| MAX_ROUTE | 50 |
| MAX_ITEMS | 100 |
| MAX_CHILDREN | 20 |

---

## 3. Module specifications

Each spec lists: the structure and its justification (your Q&A answer), what it stores, the operations it exposes, its seams, the edge cases that earn marks, and the locked implementation choice.

### Module 1 — Order Management (Member 1, Queue)

- **Structure & why:** FIFO queue. Orders are served in arrival order so none starves — fairness by construction. (A stack would serve newest-first; an unsorted list has no arrival guarantee.)
- **Stores:** `Order` records, in arrival order.
- **Operations:** add a new order to the back; remove and return the front order; report empty (nothing to process) and full (overload); display pending queue, current order, and completed history.
- **Seams:** receives new orders from intake (read from `orders.csv`); hands the removed order to Robot Assignment; on completion, the order moves to the completed history.
- **Edge cases (marks):** empty queue (return cleanly); full queue = the "system overload" case the brief names (refuse + signal); display when empty.
- **Locked:** fixed-array queue with front/rear indices (wrap-around). Track a `count`; full = count == MAX_ORDERS. Check the count before inserting — never rely on an out-of-bounds index (undefined behaviour in C++).

### Module 2 — Robot Assignment (Member 2, Circular Queue)

- **Structure & why:** circular queue. Robots rotate in a ring, wrapping from the last back to the first, spreading work evenly and never restarting the cycle.
- **Stores:** `Robot` records of *available* robots in a ring (busy robots are temporarily out).
- **Operations:** load robots (`robots.csv`); dequeue the next available robot, mark it busy, pair it with the order; enqueue it back and bump `taskCount` when it finishes; set/clear maintenance; display assignment list, current handler per task, status overview.
- **Seams:** receives the order handed out by Module 1; produces the robot↔order pairing flowing forward; on the robot's return, flips it back to available.
- **Edge cases (marks):** all robots busy = an **empty queue** (the order waits next cycle, no crash); a robot flagged maintenance is **diverted out** when dequeued (not assigned, not re-enqueued until cleared).
- **Locked:** enqueue/dequeue circular queue (front/rear wrap-around).

### Module 3 — Navigation & Path Tracking (Member 3, Stack)

This module owns the system's headline feature: returning by the exact reverse route.

- **Structure & why:** stack (LIFO). The last move out is the first to undo coming back. Reversing a path = replaying moves in reverse order (the stack does this automatically) **and** flipping each direction (you add this).
- **Stores:** `Direction` tokens — one per movement step.
- **Operations:** push a move as the robot advances; produce the return path by popping each move and emitting its opposite until empty; backtrack on an obstacle by popping; show the full navigation log; check empty (robot home).
- **Seams:** receives the route (sequence of `Location`s) from Warehouse Layout, converting each transition into a move; on task completion, pops everything to walk home, then tells Robot Assignment the robot is free.
- **Edge cases (marks):** pop on empty (already home); route longer than MAX_ROUTE (overload check); backtracking all the way to start = "no route found", report it.
- **Locked:** fixed-array stack with a top index, storing Direction tokens.

### Module 4 — Item Search (Member 4, Binary Search Tree)

- **Structure & why:** BST keyed on `itemID`. Search halves the options each step (≈O(log n)) and in-order traversal yields items already sorted — two requirements from one structure.
- **Stores:** `Item` records, ordered by itemID.
- **Operations:** insert by key; search by ID and return the item's Location; search by name (secondary, full traversal); update/delete; display sorted via in-order traversal.
- **Seams:** the "where is it?" lookup between assignment and routing — receives an order's itemID, returns that item's Location for Warehouse Layout to route to.
- **Edge cases (marks):** item not found (return cleanly); duplicate key; deleting a node with two children (find the in-order successor — the classic tricky case and a likely Q&A probe).
- **Locked:** keyed on itemID; name search is a deliberate O(n) traversal. **Q&A caveat:** a BST is only fast when reasonably balanced — inserting in sorted order degenerates it to O(n). Don't implement balancing (out of syllabus); just name the limitation if asked.

### Module 5 — Warehouse Layout (Member 5, N-ary Tree)

- **Structure & why:** general (n-ary) hierarchical tree — **not** a BST. Root = warehouse, children = zones, then aisles, then shelves. A node holds *many* children (a zone has many aisles), so left/right won't do. Because it's a tree, there's exactly one path between any two points, making routes unambiguous.
- **Stores:** the warehouse structure as nodes (zone/aisle/shelf); parent→child links are containment.
- **Operations:** build the tree (insert each item's zone/aisle/shelf path from `items.csv`); find a route — a DFS from base down to the target shelf, where the collected path *is* the route; traverse/display all sections.
- **Seams:** receives a target Location from Item Search; produces the route (sequence of Locations) for Navigation. Robots start from a fixed base (use the root) and return there.
- **Edge cases (marks):** target not in tree (invalid location); start == target (zero-length route); empty layout display.
- **Locked:** each node holds an **array of child pointers + count** (`child[MAX_CHILDREN]`). Routing is DFS.

---

## 4. How the modules connect (integration contract)

Each arrow is a handoff; the label is the standardized data passed.

```
Order Management ──[ Order {orderID, itemID} ]──▶ Robot Assignment
Robot Assignment ──[ robot ↔ order pairing ]────▶ (uses order's itemID)
Item Search      ──[ Item + Location ]──────────▶ Warehouse Layout
Warehouse Layout ──[ Route = Location[] + count ]▶ Navigation
Navigation       ──[ forward log + reverse path ]▶ (robot returns; robot freed)
```

Rules:
- Modules touch each other only through these handoffs, never each other's internals.
- All sequences travel as **fixed array + count**.
- `main.cpp` (driver) wires the modules together in this order; stub it early so modules have something to plug into as they come online.

---

## 5. Conventions

**Naming**
- Variables & functions: camelCase (`enqueueOrder`, `frontIndex`)
- Types/structs/classes: Capitalised (`Robot`, `OrderQueue`)
- Constants: UPPER_SNAKE (`MAX_ORDERS`)
- Meaningful names throughout — no `x`, `tmp`, `q2` (graded).

**Files**
- One `.hpp` + `.cpp` pair per module, named after it.
- `Types.hpp` for the shared model; `main.cpp` for the driver.

**Comments & formatting**
- One-line comment above each function stating what it does.
- Inline comments on the fiddly parts.
- No file-header block.
- One shared indentation: 4 spaces (indentation is on the marking sheet).

**Data files** (comma-delimited, one record per line, no header row)
- `orders.csv` → `orderID,itemID` (e.g. `101,55`) — status starts pending in code
- `robots.csv` → `robotID,status` (e.g. `1,available`) — status: available / busy / maintenance
- `items.csv` → `itemID,name,zone,aisle,shelf` (e.g. `55,Widget,1,3,5`)
- No separate layout file — Member 5 builds the tree from the locations in `items.csv`.
