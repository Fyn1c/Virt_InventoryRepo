import { useState } from "react";
import { TableGootsRegister } from "./components/table_goots_register/TableGootsRegister";
import { Jornal } from "./components/jornal/Jornal";
import { WriteDowns } from "./components/write_downs/WriteDowns";
import { Entrance } from "./components/entrance/Entrance";
import { Moving } from "./components/Moving/Moving";
import { Search } from "./components/search/Searct";

const goods = [
  { id: 1, name: "ручка", status: "в наличии", remains: 0, actions: "-" },
  { id: 2, name: "карандаш", status: "в наличии", remains: 0, actions: "-" },
  { id: 3, name: "тетрадь", status: "в наличии", remains: 0, actions: "-" },
];

const jornal = [
  { id: 1, name: "ручка",date: "01.02.26",type: "канцелярия",quantity: 8, status: "в наличии"},
  { id: 2, name: "карандаш",date: "01.02.26",type: "канцелярия",quantity: 7, status: "в наличии"},
  { id: 3, name: "пенал",date: "01.02.26",type: "канцелярия",quantity: 6, status: "в наличии"},
];

export function App() {
  const [page, setPage] = useState<"register" | "journal">("register");
  const [search, setSearch] = useState("");
  type ActiveModal = "none" | "entrance" | "moving" | "write_downs";
  const [activeModal, setActiveModal] = useState<ActiveModal>("none");

  return (
    <div style={{ fontFamily: "Arial, sans-serif" }}>
    <Search onSearch={setSearch} />

    <div className="App">
      <nav style={{ display: "flex", gap: "12px", marginBottom: "16px" }}>
        <button type="button" onClick={() => setPage("register")}>
          Реестр товаров
        </button>
        <button type="button" onClick={() => setPage("journal")}>
          Журнал
        </button>
        <button type="button" onClick={() => setActiveModal("write_downs")}>
          Списание
        </button>

        <WriteDowns
          isOpen={activeModal == "write_downs"}
          productName="ручка"
          currentStock={5}
          onClose={()=>setActiveModal("none")}
          onSubmit={(qty)=>console.log("Списание", qty)}
        />
        <button type="button" onClick={() => setActiveModal("entrance")}>
          Приход
        </button>

        <Entrance
          isOpen={activeModal == "entrance"}
          productName="ручка"
          onClose={()=>setActiveModal("none")}
          onSubmit={(qty)=>console.log("Приход", qty)}
        />
        <button type="button" onClick={() => setActiveModal("moving")}>
          Перемещение
        </button>

        <Moving
          isOpen={activeModal == "moving"}
          productName="ручка"
          onClose={()=>setActiveModal("none")}
          onSubmit={(status)=>console.log("Перемещение", status)}
        />
      </nav>
      {page === "register" ? <TableGootsRegister data={goods} /> : <Jornal data={jornal} />}
      </div>
    </div>
  );
}

export default App;

