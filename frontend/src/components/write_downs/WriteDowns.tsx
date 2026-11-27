import { useState, type FormEvent } from "react";

interface WriteDownsProps {
  isOpen: boolean;
  currentStock: number;
  onClose: () => void;
  onSubmit?: (quantity: number) => void;
}

export function WriteDowns({
  isOpen,
  currentStock,
  onClose,
  onSubmit,
}: WriteDownsProps) {
  const [productName, setProductName] = useState<string>("");
  const [quantity, setQuantity] = useState<string>("1");
  const [error, setError] = useState<string>("");

  if (!isOpen) return null;

  const handleSubmit = (e: FormEvent) => {
    e.preventDefault();
    const qty = Number(quantity);

    setError("");
    onSubmit?.(qty);
    onClose();
  };

  return (
    <div
        style={{
            position:"fixed",
            inset:0,
            display:"flex",
            alignItems:"center",
            justifyContent:"center",
            zIndex:1000,
            backgroundColor: "rgba(0, 0, 0, 0.5)",
        }}
    >
      <form onSubmit={handleSubmit}
        style={{
            background: "#fff",
            padding: 24,
            borderRadius: 8,
            width: "100%",
            maxWidth: 400,         
            boxSizing: "border-box",
        }}
      >
        <h3>Списание:</h3>
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
        <p>Остаток: {currentStock}</p>

          Количество:
          <input
            type="number"
            value={quantity}
            onChange={(e) => {
              setQuantity(e.target.value);
            }}
          />
        </label>
        {error && <span>{error}</span>}
        <div>
          <button type="submit">Расход</button>
          <button type="button" onClick={onClose}>
            Отмена
          </button>
        </div>
      </form>
    </div>
  );
}