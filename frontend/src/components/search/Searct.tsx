import { useState, type FormEvent } from "react";

interface HeaderProps {
  onSearch: (query: string) => void;
}

export function Search({ onSearch }: HeaderProps) {
  const [query, setQuery] = useState("");

  const SearchChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = e.target.value;
    setQuery(value);
    onSearch(value); // отдаём строку родителю на каждый ввод
  };

  const SearchSubmit = (e: FormEvent) => {
    e.preventDefault();
    onSearch(query); // на случай, если поиск по кнопке
  };

  return (
    <header
      style={{
        display: "flex",
        alignItems: "center",
        justifyContent: "space-between",
        gap: 16,
        padding: "12px 20px",
        borderBottom: "1px solid #eee",
      }}
    >
      <h3 style={{ margin: 0 }}>Склад</h3>

      <form onSubmit={SearchSubmit}>
        <input
          type="text"
          value={query}
          onChange={SearchChange}
          placeholder="Поиск по наименованию…"
        />
      </form>
    </header>
  );
}
