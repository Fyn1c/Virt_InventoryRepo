import React from 'react';
//import './table.css';

type Jornals = {
  id: number;
  date: string;
  name: string;
  type: string;
  quantity: number;
  status: string;
};


export const Jornal: React.FC<{ data: Jornals[] }> = ({ data }) => {
  return (
    <table>
      <thead>
        <tr>
          <th>ID</th>
          <th>Дата</th>
          <th>Товар</th>
          <th>Тип</th>
          <th>Количество</th>
          <th>Статус-результат</th>
        </tr>
      </thead>
      <tbody>
        {data.map((item) => (
          <tr key={item.id}>
            <td>{item.id}</td>
            <td>{item.date}</td>
            <td>{item.name}</td>
            <td>{item.type}</td>
            <td>{item.quantity}</td>
            <td>{item.status}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
};