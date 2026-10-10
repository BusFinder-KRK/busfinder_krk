import "../../style/components/layout/Header.css"
import InfoButton from "../minor/InfoButton";

export default function Header() {
    return (
        <div className="hdr">
            <h1>
                BusFinder
            </h1>
            <InfoButton />
        </div>
    );
}