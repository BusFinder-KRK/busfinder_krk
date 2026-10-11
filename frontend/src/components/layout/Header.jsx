import "../../style/components/layout/Header.css"
import "../../style/components/Info.css"

export default function Header({ onOpenInfo }) {
    return (
        <div className="hdr">
            <h1>
                BusFinder
            </h1>
            <button onClick={onOpenInfo} className="info-button" />
        </div>
    );
}