import { useState, type FormEvent } from "react";

export type Status = "На складе" | "выдано" | "в резерве";

interface MovingProps {
  isOpen: boolean;
  onClose: () => void;
  onSubmit?: (status: Status) => void;
}

export function Moving({
  isOpen,
  onClose,
  onSubmit,
}: MovingProps) {
  const [productName, setProductName] = useState<string>("");
  const [status, setStatus] = useState<Status | "">("");  // хук внутри компонента
  const [error, setError] = useState<string>("");

  if (!isOpen) return null;

  const handleSubmit = (e: FormEvent) => {
    e.preventDefault();

    const name = productName.trim();
    if (name === "") {
      setError("Введите наименование товара");
      return;
    }

    if (!status) {
      setError("Выберите статус");
      return;
    }

    setError("");
    onSubmit?.(status);
    onClose();
  };

  return (
    <div
      style={{
        position: "fixed",
        inset: 0,
        display: "flex",
        alignItems: "center",
        justifyContent: "center",
        zIndex: 1000,
        backgroundColor: "rgba(0, 0, 0, 0.5)",
      }}
    >
      <form
        onSubmit={handleSubmit}
        style={{
          background: "#fff",
          padding: 24,
          borderRadius: 8,
          width: "100%",
          maxWidth: 400,
          boxSizing: "border-box",
        }}
      >
        
        <h3>Перемещение: {productName}</h3>
        <label>
          Наименование товара:
          <input
            type="text"
            value={productName}
            onChange={(e) => {
              setProductName(e.target.value);
              if (error) setError("");
            }}
            placeholder="Введите имя товара"
          />
          Статус:
          <select
            value={status}
            onChange={(e) => {
              setStatus(e.target.value as Status | "");
              if (error) setError("");
            }}
          >
            <option value="" disabled>
              – Выберите статус –
            </option>
            <option value="На складе">На складе</option>
            <option value="выдано">Выдано</option>
            <option value="в резерве">В резерве</option>
          </select>
        </label>
        {error && <span>{error}</span>}

        <div>
          <button type="submit">Перемещение</button>
          <button type="button" onClick={onClose}>
            Отмена
          </button>
        </div>
      </form>
    </div>
  );
}
