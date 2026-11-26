import React from 'react';
//import './table.css';

type Goods = {
  id: number;
  name: string;
  status: string;
  remains: number;
  actions: string;
};

export const TableGootsRegister: React.FC<{ data: Goods[] }> = ({ data }) => {
  return (
    <table>
      <thead>
        <tr>
          <th>ID</th>
          <th>Товар</th>
          <th>Статус</th>
          <th>Остаток</th>
          <th>Действие</th>
        </tr>
      </thead>
      <tbody>
        {data.map((item) => (
          <tr key={item.id}>
            <td>{item.id}</td>
            <td>{item.name}</td>
            <td>{item.status}</td>
            <td>{item.remains}</td>
            <td>{item.actions}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
};