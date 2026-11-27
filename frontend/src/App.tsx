import { useState, useEffect } from "react";
import { TableGootsRegister, type Goods } from "./components/table_goots_register/TableGootsRegister";
import { Jornal, type Jornals } from "./components/jornal/Jornal";
import { WriteDowns } from "./components/write_downs/WriteDowns";
import { Entrance } from "./components/entrance/Entrance";
import { Moving } from "./components/Moving/Moving";
import { Search } from "./components/search/Searct";

/*const goods = [
  { id: 1, name: "ручка", status: "в наличии", remains: 0, actions: "-" },
  { id: 2, name: "карандаш", status: "в наличии", remains: 0, actions: "-" },
  { id: 3, name: "тетрадь", status: "в наличии", remains: 0, actions: "-" },
];

const jornal = [
  { id: 1, name: "ручка",date: "01.02.26",type: "канцелярия",quantity: 8, status: "в наличии"},
  { id: 2, name: "карандаш",date: "01.02.26",type: "канцелярия",quantity: 7, status: "в наличии"},
  { id: 3, name: "пенал",date: "01.02.26",type: "канцелярия",quantity: 6, status: "в наличии"},
];
*/
export function App() {
  const [page, setPage] = useState<"register" | "journal">("register");
  //const [search, setSearch] = useState(""); компилятор ругается на то что search не использован потом обратно добавить при надобности
  const [, setSearch] = useState("");
  type ActiveModal = "none" | "entrance" | "moving" | "write_downs";
  const [activeModal, setActiveModal] = useState<ActiveModal>("none");
  const [goods, setGoods] = useState<Goods[]>([]);
  const [jornal, setJornal] = useState<Jornals[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string>("");

  useEffect(() => {
    async function loadData() {
      try {
        const [goodsRes, jornalRes] = await Promise.all([
          fetch("http://localhost:3000/goods"),//вставить реальный адрес
          fetch("http://localhost:3000/jornal"),//вставить реальный адрес
        ]);

        if (!goodsRes.ok || !jornalRes.ok) {
          throw new Error("Ошибка загрузки данных");
        }

        setGoods(await goodsRes.json());
        setJornal(await jornalRes.json());
      } catch (e) {
        setError("Не удалось загрузить данные с сервера");
        console.error(e);
      } finally {
        setLoading(false);
      }
    }

    loadData();
  }, []); 
  if(loading){
    return <div>Загрузка</div>;
  }
  if (error) {
    return <div style={{ padding: 20, color: "red" }}>{error}</div>;
  }

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
          currentStock={5}
          onClose={()=>setActiveModal("none")}
          onSubmit={(qty)=>console.log("Списание", qty)}
        />
        <button type="button" onClick={() => setActiveModal("entrance")}>
          Приход
        </button>

        <Entrance
          isOpen={activeModal == "entrance"}
          onClose={()=>setActiveModal("none")}
          onSubmit={(qty)=>console.log("Приход", qty)}
        />
        <button type="button" onClick={() => setActiveModal("moving")}>
          Перемещение
        </button>

        <Moving
          isOpen={activeModal == "moving"}
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

